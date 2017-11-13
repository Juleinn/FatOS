/** @file fs.h
 *  @author Anton CLAES
 *  @date 2017
    @brief Contains all functions for filesystem handling
 */

#ifndef FS_H_
#define FS_H_

#include "floppy.h"
#include "string.h"
#include "memory.h"

/**
@brief Structure describing filelist on the FatOS filesystem.
*/
struct FileList{
	/** The buffer containing the actual filelist data */
	char * data;
	/** The length of the data in bytes */
	int length;
	/** The first sector of the filelist for later rewrite on drive */
	unsigned short firstSector;
};
typedef struct FileList FileList;

/**
@brief Structure describing a FatOS filesystem. This allows 
for multiple filesystems to be supported (with multiple drives) and handled as a linked list */
struct FileSystem{
	/** The drive ID holding the fs */
    int drive_id;					
    /** pointer to the FAT for that filesystem */
    char * FATpointer;  			
    /** allows for easy chaining of filesystems */
    struct FileSystem * next;   	
    /** The filelist of the root directory */
    FileList root;
};
typedef struct FileSystem FileSystem;

#include "sysfat.h"
#include "sysdir.h"

/**
@brief the numbers of sectors in the FAT
*/
#define FAT_SECTORS 10
/**
@brief the id of the sector at which FAT is located (in LBA)
*/
#define FAT_FIRST 1

// 
/**
@brief initializes all filesystems accessible (one per detected floppy drive). 
SO FAR : ONLY boot filesystem
*/
void filesystems_init();

/**
@brief init file system (especially load file table at the given address)
@param fs the filesystem to initialize
*/
void filesystem_init(FileSystem * fs);

/**
@brief Reads the given amount of bytes from file given at first sector into provided buffer.
@param firstSector the firstSector of the file
@param startByte the firstByte to read
@param length the number of bytes to read
@param a buffer to read into
@param fs the filesystem to read from
@return -1 upon error, 0 upon success
*/
int filesystem_readbytesByFirstSector(unsigned short firstSector, int startByte,
                                        int length, char* buffer, FileSystem * fs);

/**
@brief Writes the given amount of bytes from given buffer at sector indicated
  returns -1 upon error, 0 upon success 
	- firstSector the first sector of the file 
	- startByte the first byte to write too
	- length the number of bytes to write
	- fs the filesystem to write into

	If the required data writes beyond file limit then the function will fail. It will not
	append sectors to the file. 
@param firstSector the firstSector of the file
@param startByte the firstByte to write
@param length the number of bytes to write
@param a buffer to read from
@param fs the filesystem to write to
@return -1 upon error, 0 upon success
  */
int filesystem_writebytesByFirstSector(unsigned short firstSector, int startByte,
                                        int length, char* buffer, FileSystem * fs);

/**
@brief Gets the first filesystem handled by the filesystem handler (boot filesystem)
@return a pointer a filesystem structure representing the first filesystem
*/
FileSystem * getFirstFileSystem();


/** 
@brief Returns a pointer to the filesystem of given drive ID
@param id the drive's id
@return a pointer to the filesystem's structure */
FileSystem * getFileSystemByDriveId(int id);

/**
@brief Sets the next sector in the FAT loaded in memory (does not rewrite it in drive)
@param start the sector to set the next one to
@param next the sector to be written as following the "start" one
@param fs the filesystem on whose FAT to operate changes on
*/
void filesystem_setNextSector(unsigned short start, unsigned short next, FileSystem * fs);

/** 
@brief This finds an empty sector to write to, after a given first one (initial value, set to 0 if
all drive)
@param first to first sector after which to look for an empty one
@param fs the filesystem on which to perform operations
@return the sector found (0 upon failure)
*/
unsigned short filesystem_findEmptySector(unsigned short first, FileSystem * fs);

/**
@brief This adds sectors at the end of chained list of sectors in FAT. Rewrites FAT into drive
if specified so. 
@param first a sector in a chain at the end of which to add sectors
@param count the number of sectors to add
@param rewrite : 1 to rewrite FAT to disk, 0 to just update memory
@param fs the filesystem on which to operate changes
@return 0 upon success 
*/
int filesystem_appendSectors(unsigned short first, int count, int rewrite, FileSystem * fs);

/**
@brief Appends bytes to the selected file 
@param fs the filesystem on which to operate changes 
@param fl the filelist
@param filename the name of the files to add bytes to
@param bytesToAppend the amount of bytes to be appened
@this modifies FAT and filelist where the file is identified
*/
int filesystem_appendBytes(FileSystem * fs, FileList fl,
    char * filename, int bytesToAppend);

/**
@brief Lists the contents of a given directory.
End user function, supports complete path, including drive id.
Supports 1-digits drives ids (0-9)
@param fullPath the absolute path to the directory
@return -1 upon failure
*/
int filesystems_driveList(char * fullPath);


/**
@brief returns a pointer to a FAT entry of given FAT and entry id
@param fatPtr a pointer to the FAT in memory
@param id the id of the entry (indexed start at 0)
*/
#define FAT_ENTRYAT(fatPtr, id) (*((unsigned short*)((int) fatPtr + 2*id)))

/**
@brief computes the number of sectors of 512 bytes required to hold the given amount of bytes
@param bytes the number of bytes
@return the number of sectors
*/
#define REQ_SECTORS(bytes) ((bytes) % 512 == 0 ? (bytes) / 512 : ((bytes) / 512) + 1)

#endif

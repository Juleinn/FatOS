/* File fs.h
 * Anton CLAES
 * filesystem handling
 */

#ifndef FS_H_
#define FS_H_

#include "floppy.h"
#include "string.h"
#include "memory.h"

/* This allows for multiple filesystems to be supported (with multiple drives) */
struct FileSystem{
    int drive_id;
    char * FATpointer;  // pointer to the FAT for that filesystem
    struct FileSystem * next;   // allows for easy chaining of filesystems
    char * fileList;            // a pointer to the fileList loaded into memory
    int fileListSize;
};
typedef struct FileSystem FileSystem;

// retrieves the next sector to read from based
// from FAT (sector 1) loaded into buffer
unsigned short filesystem_getNextSector(unsigned short sector, FileSystem * fs);

// same as above without striping the 'used' flag
unsigned short filesystem_getNextSectorRaw(unsigned short sector, FileSystem * fs);

// first sector : n=0 (indexing starts at 0)
unsigned short filesystem_getNthSector(unsigned short firstSector, int n, FileSystem * fs);

// size of the file table in sectors
#define FAT_SECTORS 10
// FAT first sector on disk in LBA
#define FAT_FIRST 1

// initializes all filesystems accessible (one per detected floppy drive)
// SO FAR : ONLY boot filesystem
void filesystems_init();

// init file system (especially load file table at the given address)
void filesystem_init(FileSystem * fs);

// computes number of sector a file actual uses (based on FAT, not on advertised size)
// based on the first sector
// reads it from FAT loaded into memory
unsigned short filesystem_getFileSectors(unsigned short firstSector, FileSystem * fs);

// reads the given amount of bytes from file given at first sector
// returns -1 upon error, 0 upon success
int filesystem_readbytesByFirstSector(unsigned short firstSector, int startByte,
                                        int length, char* buffer, FileSystem * fs);

FileSystem * getFirstFileSystem();

/* Loads the filetable for a given filesystem */
void filesystem_LoadFileList(FileSystem * fs);

/* Returns a pointer to the filesystem of given drive ID */
FileSystem * getFileSystemByDriveId(int id);

/* Prints the content of a given fileList (loaded at given position in memory, with given size) */
void filesystem_list(char* fileListData, int length);

// directory format :
// drive_id/subdir/subdir/file(.ext)


#endif

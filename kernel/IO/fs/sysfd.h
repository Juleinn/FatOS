/** @file sysfd.h
 *  @author Anton CLAES
 *  @date 2017
    @brief Contains filesystem functions for handling file descriptors for open files
 */
#ifndef SYSFD_H
#define SYSFD_H

#include "memory.h"
#include "print.h"
#include "fs.h"

/**
@brief Describes an open file 
*/
struct FileDescriptor{
	/** The full path to the file */
	char 			filename[256];
	/** The first sector of the file on disk in LBA */
	unsigned short 	firstSector;
	/** Flags -bit 0 : open, bit 1 : read, bit 2 : write, bit 3-7:unused */
	unsigned char 	flag;
	/** Current position of cursor in flag*/
	unsigned short 	currentPos;
	/** Pointer to the filesystem on which the file is stored */
	FileSystem * fs; 
};
typedef struct FileDescriptor FileDescriptor;

/** 
@brief maximal number of file descriptors open at the same time
**/
#define SYSFD_MAX_OPEN_FDS 1500

/**
@brief initializes the file descriptors system
*/
void sysfs_init_fds();

/**
@adds a file descriptor to the list
@param fd the filedescriptor to add
@return index of filedescriptor upon success, -1 upon failure
*/
int sysfd_addfd(FileDescriptor fd);

/**
@brief destroys the given FileDescriptor by its index
@param index the index of the filedescriptor to destroy
@return 0 upon success, -1 upon failure
*/
int sysfd_rmfd(int index);

/**
@brief retrieves the file descriptor at given index
@param fdId the index
@return a pointer to the file descriptor
*/
FileDescriptor * sysfd_getFd(int index);

/**
@brief Creates a filedescriptor based on filename and returns its id
@param filename the filename of the file to create descriptor for
@param write : 1 = open for writing/appending 0 = readonly
@param firstSector the first sector of the file on the disk
*/
int sysfd_createfd(char * filename, int write, int firstSector);

/**
@brief Prints file descriptor table (for debugging purposes mainly).
*/
void sysfd_printfdlist();

#endif
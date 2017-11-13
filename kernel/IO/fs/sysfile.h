/**
@file sysfile.h
@brief contains file opening/reading/writing/closing functions for the system to use
@author Anton Claes
@date 2017
*/

#ifndef SYSFILE_H_
#define SYSFILE_H_

#include "fs.h"
#include "sysfd.h"
#include "sysdir.h"

/**
@brief open file in read mode
*/
#define SYSFD_MODE_READ 0

/**
@brief open file in write mode
*/
#define SYSFD_MODE_WRITE 1

/**
@brief initilize the usage of files (init filesystem and file descriptors)
*/
void sysfile_init();

/**
@brief looks up the given filename and opens it if possible
@param filename the full path to the desired file
@param mode : 0 for read, 1 for write
@return the id of the file descriptor upon success, -1 upon failure
*/
int sys_fopen(const char * filename, int mode);

/**
@brief reads the given amount of bytes into the given buffer
@param fdId the id of the file descriptor returned by sys_fopen
@param buffer the buffer to write to
@param startByte the first byte to read from (index from the beginning of the file)
@param length the number of bytes to read
@return the number of bytes actually read
*/
int sys_fread(int fdId, char * buffer, int startByte, int length);

/**
@brief writes the amount of bytes from given buffer into file
@param fdId the id of the file to write to
@param buffer the buffer to read from
@param startByte the start byte to write to in FILE
@param length the number of bytes to write
@return the number of bytes written
*/
int sys_fwrite(int fdId, char * buffer, int startByte, int length);

/**
@brief closes the given file
@param fdId the file to close
*/
void sys_fclose(int fdId);

/**
@brief returns the index of string of end of path. Ex : /dir/file.bin : 5
@param path the path to the file
@return the index of the beginning of the filename
*/
int sys_getPathFileIndex(char * path);

#endif
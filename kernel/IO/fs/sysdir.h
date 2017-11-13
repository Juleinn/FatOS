/** @file sysdir.h
 *  @author Anton CLAES
 *  @date 2017
    @brief Contains filesystem functions for handling directories
 */
#ifndef SYSDIR_H_
#define SYSDIR_H_

#include "fs.h"

/**
@brief Loads the filetable for a given filesystem 
@param fs the filesystem for which to load the root filelist (located at sector 11)
*/
void filesystem_LoadRootFileList(FileSystem * fs);

/** 
@brief Prints the content of a given fileList (loaded at given position in memory, with given size) 
@param fl the FileList
*/
void filesystem_list(FileSystem * fs, FileList fl);

// directory format :
// drive_id/subdir/subdir/file(.ext)

/**
@brief Find first sector of entry of specified name in table loaded in buffer, of given size 
@param filename a string representing the filename in the filelist
@param fl the filelist
@return the first sector of the file as advertised in the filelist
*/
unsigned short filesystem_findFirstSector(char * filename, FileList fl);

/**
@brief Returns the number of bytes of a file in a given filelist. 
@param fl the filelist
@param filename the name of the file
@return -1 if not found, the number of bytes of the files as indicated by the filelist otherwise*/
int filesystem_getBytes(FileList fl, char* filename);

/**
@brief sets the number of bytes of file in a given filelist.
@param fl the filelist
@param filename the name of the file
@param newSize the size to be set to the file
@return -1 if not found
*/
int filesystem_setBytes(FileList fl, char* filename, 
	int newSize);

/**
@brief expands a filelist by given path
@param path the path to the filelist to expand
@param size the size to set for the filelist
@return 0 upon success, 1 upon error
*/
int filessytem_addFilelistBytes(const char * path, int bytes);

/**
@brief loads a subfilelist into an allocated buffer. 
This buffer needs to be freed after using sys_free.
@param fs the filesystem from which to load the sub filelist
@param fl the filelist
@param subFileListName the name of the subFileList(directory to look for)
@return the size of the loaded subfilelist in bytes, -1 if failed
*/
FileList filesystem_loadSubFileListByName(FileSystem * fs, FileList fl, 
	char * subFileListName);

/**
@brief loads a filelist from a given absolute path
@param fullpath the full path of the sublist
@return fl the loaded filelist
*/
FileList filesystem_LoadRootFileListByPath(char * fullpath);

/**
@brief loads a subfileListFrom the current filelist
@param fs the filesystem
@param fileList the current filelist in which to look for
@param subListPath the remaining path 
*/
FileList filesystem_LoadSubFileList(FileSystem * fs, FileList fl, char * subListPath, int level);

/**
@brief Lists through a path ex. /subdir/ recursively until destination reached. 
@param fs the filesystem into which to look for the sublist
@param fl the filelist
@param subListName the name of the sublist to look for
@return -1 upon failure, 0 upon success 
*/
int filesystem_subList(FileSystem * fs, FileList fl,
	char * subListName);

/**
@brief Frees a filelist's data buffer
@param fl the filelist containing the buffer to free
*/
void filesystem_fileListFreeBuffer(FileList fl);

/**
@brief creates a new file/dir at given filename
@param fullpath the full path of the file to create
@param dirFlag : 0 = create regular file, non-zero = directory
@return 0 upon success, -1 upon failure (most likely )
*/
int filesystem_createfile(const char * fullpath, int dirFlag);

/**
@brief find the offset of the first possible empty spot in the filelist
@param filelist the filelist to analyse
@param size the total size of the entry to add
@return 0 upon success, -1 upon failure
*/
int filesystem_findemptyFilelistSpot(FileList fl, int size);

/**
@brief find the parent directory path given a path. Handles the case where 
the current dir is root dir
@param path the path to analyse. /!\ will be modified /!\
@return 1 if parent is root dir (useless ?), 0 otherwise
*/
int filesystem_cutparentdirectory(char * filename);


#endif
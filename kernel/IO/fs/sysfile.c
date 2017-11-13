/**
@file sysfile.h
@brief contains file opening/reading/writing/closing functions for the system to use
@author Anton Claes
@date 2017
*/
#include "sysfile.h"

void sysfile_init()
{
	filesystems_init();
	sysfs_init_fds();
}

int sys_fopen(const char * filename, int mode)
{
	/* load the filelist in which file is loaded */
	char listPath[256];
	strcpy(filename, listPath);
	int fileNameIndex = sys_getPathFileIndex(filename);
	listPath[fileNameIndex]='\0';
	FileList fl = filesystem_LoadRootFileListByPath(listPath);
	int firstSector = filesystem_findFirstSector(filename + fileNameIndex, fl);

	if(firstSector != 0)
		// could access first sector of file
		return sysfd_createfd(filename, mode, firstSector);
	else
		return -1;
}

int sys_fread(int fdId, char * buffer, int startByte, int length)
{
	/* Retrieve the fdlist entry */
	FileDescriptor * fd = sysfd_getFd(fdId);
	return filesystem_readbytesByFirstSector(fd->firstSector, startByte, length, buffer, fd->fs);
}

int sys_fwrite(int fdId, char * buffer, int startByte, int length)
{

}

void sys_fclose(int fdId)
{

}

int sys_getPathFileIndex(char * path)
{
	int lastSep = 0;
	int index = 0;
	while(path[index])
	{
		if(path[index] == '/')
			lastSep = index;
		index++;
	}
	return lastSep+1;
}
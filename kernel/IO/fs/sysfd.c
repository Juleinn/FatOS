/** @file sysfd.h
 *  @author Anton CLAES
 *  @date 2017
    @brief Contains filesystem functions for handling file descriptors for open files
 */

#include "sysfd.h"

/* List of all open sys file descriptors for the system */
FileDescriptor * sysfd_list;


void sysfs_init_fds()
{
	sysfd_list = (FileDescriptor*) sys_alloc(sizeof(FileDescriptor) * SYSFD_MAX_OPEN_FDS);
	int i;

	FileDescriptor defaultFd;
	defaultFd.filename[0] = 0;	// empty string
	defaultFd.firstSector = 0;
	defaultFd.flag = 0x00;		// closed file
	defaultFd.currentPos = 0;

	for(i=0; i<SYSFD_MAX_OPEN_FDS; i++)
		sysfd_list[i] = defaultFd;
}

int sysfd_addfd(FileDescriptor fd)
{
	/* Find an empty spot and add the filedescriptor */
	int i;
	for(i=0; i<SYSFD_MAX_OPEN_FDS; i++)
	{
		// find free spot 
		if(!(sysfd_list[i].flag & 0x01))
		{
			sysfd_list[i] = fd;
			return i;
		}
	}
	return -1;
}

int sysfd_rmfd(int index)
{
	int i;
	/* Check that it is actually open */
	if(sysfd_list[index].flag & 0x01)
	{
		sysfd_list[index].flag = 0x00; /* Wipe 'open' bit*/ 
		return 0;
	}
	return -1;
}

int sysfd_createfd(char * filename, int write, int firstSector)
{
	FileDescriptor fd; 

	if(write)
	{
		/* Need to check if the same file is already open in write mode */
		int i;
		for(i=0; i<SYSFD_MAX_OPEN_FDS; i++)
		{
			/* write mode and open file descriptor */
			if(sysfd_list[i].flag & 0x04 
				&& sysfd_list[i].flag & 0x01
				&& strcmp(filename, sysfd_list[i].filename) == 0)
			{
				return -1;
			}
		}
	}

	/* Init. the file descriptor */
	strcpy(filename, fd.filename);
	fd.firstSector = firstSector;
	fd.flag = 0x01;
	fd.flag |= write ? 0x06 : 0x02;
	fd.fs = getFileSystemByDriveId(filename[0] - '0');
	fd.currentPos = 0;
	sysfd_addfd(fd);
}

void sysfd_printfdlist()
{
	int i;
	for(i=0; i<SYSFD_MAX_OPEN_FDS; i++)
	{
		/* used entries only */
		if(sysfd_list[i].flag & 0x01)
		{
			printi(i);prints(":");
			prints(sysfd_list[i].filename);
			prints(":");
			printi(sysfd_list[i].firstSector);
			prints(":");
			if(sysfd_list[i].flag & 0x02) prints("r");
			if(sysfd_list[i].flag & 0x04) prints("w");
			prints("\n");
		}
	}
}

FileDescriptor * sysfd_getFd(int index)
{
	return sysfd_list + index;
}


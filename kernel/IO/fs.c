/**
@file fs.c
@author Anton Claes
@date 2017
@brief contains the implementation for the filesystem functions described in fs.h
*/
#include "fs.h"

/* Pointer to first filesystem to be loaded (boot filesystem)*/
FileSystem * firstFileSystem;

// can be replaced by macro later
unsigned short filesystem_getNextSector(unsigned short sector, FileSystem * fs)
{
    // just read the sector value contained in FAT
    // does not perform any verification on data, just strips used flag
    return (*((unsigned short*)((int) fs->FATpointer + 2*sector))) & 0x7FFF;
}

unsigned short filesystem_getNextSectorRaw(unsigned short sector, FileSystem * fs)
{
    return (*((unsigned short*)((int) fs->FATpointer + 2*sector)));
}

void filesystems_init()
{
    // create boot filesystem entry
    firstFileSystem = sys_alloc(sizeof(FileSystem));
    // assumes boot drive is 0 (needs to be changed later)
    firstFileSystem->drive_id = 0;
    firstFileSystem->next = (void*) 0;
    filesystem_init(firstFileSystem);
    // prints("Initialized filesystems\n");
}

void filesystem_init(FileSystem * fs)
{
  // generate a 10 sectors buffer
  fs->FATpointer = (char*) sys_alloc(FAT_SECTORS * SECTOR_SIZE);

  // actually load the sectors
  int i;
  for(i=0; i<FAT_SECTORS; i++)
  {
      floppy_readsectorinto(i + FAT_FIRST, fs->drive_id, (char*)((int) fs->FATpointer + (SECTOR_SIZE * i)));
  }
  // // display the first few bytes of data read
  // for(i=0; i<10; i++)
  // {
  //     unsigned short * data = (unsigned short*)((int) fs->FATpointer + 2*i);
  //     if(*data & 0x8000)
  //       prints("u:next=");
  //     else
  //       prints("f:next=");
  //
  //     printh(*data & 0x7FFF);prints("\n");
  // }

  // load the filelist
  filesystem_LoadFileList(fs);
}

unsigned short filesystem_getFileSectors(unsigned short firstSector, FileSystem * fs)
{
    // just loop through until end of file
    // impose maximum number in case FAT is corrupt
    // no not read anything outside memory

    unsigned short n = 1; // number of sectors
    unsigned short currentSector = firstSector;

    // check if first sector valid :
    if((*((unsigned short*)((int) fs->FATpointer + 2*firstSector))) & 0x8000)
    {
        while(filesystem_getNextSector(currentSector, fs) != 0)
        {
            n++;
            currentSector = filesystem_getNextSector(currentSector, fs);
        }
        return n;
    }
    else
    {
        prints("Invalid first sector\n");
        return 0;
    }
}

unsigned short filesystem_getNthSector(unsigned short firstSector, int n, FileSystem * fs)
{
    unsigned short currentSector = firstSector;
    int i;
    for(i=0; i<n; i++)
    {
        currentSector = filesystem_getNextSector(currentSector, fs);
        // prints("currentSector = ");printi((int)currentSector);prints("\n");
    }
    return currentSector;
}

int filesystem_readbytesByFirstSector(unsigned short firstSector, int startByte, int length, char* buffer, FileSystem * fs)
{
    // as this starts to become higher level functions, perform some checks
    int sectorsNeeded = (startByte + length) % SECTOR_SIZE == 0 ?
                (startByte + length)/SECTOR_SIZE : ((startByte + length)/SECTOR_SIZE) + 1;

    if(sectorsNeeded > filesystem_getFileSectors(firstSector, fs))
    {
        prints("Warning : trying to read after end of file\n");
        return 0;
    }

    firstSector = filesystem_getNthSector(firstSector, startByte/SECTOR_SIZE, fs);

    // compute start buffer and start offset
    startByte %= SECTOR_SIZE; // for read from first sector
    int endByte = (length + startByte) % SECTOR_SIZE == 0 ? SECTOR_SIZE : (length + startByte) % SECTOR_SIZE;
    int offset = 0;

    int i=0;
    for(i=0; i<sectorsNeeded; i++)
    {
        int start = i == 0 ? startByte : 0; // if first sector
        int end = i == sectorsNeeded - 1 ? endByte : SECTOR_SIZE; // last sector
        // prints("Reading sector ");printi(firstSector+i);prints(" from byte ");printi(start);
        // prints(" to ");printi(end);prints("\n");
        char * flpBuf = floppy_readsector(firstSector+i, fs->drive_id);
        // copy appropriate data
        memcpy(flpBuf + start, buffer + offset, end - start);
        offset += end - start;
    }
    return offset;

}

int filesystem_writebytesByFirstSector(
    unsigned short firstSector, 
    int startByte,
    int length, 
    char* buffer,
    FileSystem * fs)
{
    // as this starts to become higher level functions, perform some checks
    int sectorsNeeded = (startByte + length) % SECTOR_SIZE == 0 ?
                (startByte + length)/SECTOR_SIZE : ((startByte + length)/SECTOR_SIZE) + 1;

    if(sectorsNeeded > filesystem_getFileSectors(firstSector, fs))
    {
        prints("Warning : trying to write after end of file\n");
        return 0;
    }

    /* First sector to write into */
    firstSector = filesystem_getNthSector(firstSector, startByte/SECTOR_SIZE, fs);

    // compute start buffer and start offset
    startByte %= SECTOR_SIZE; // for read from first sector
    int endByte = (length + startByte) % SECTOR_SIZE == 0 ? SECTOR_SIZE : (length + startByte) % SECTOR_SIZE;
    int offset = 0;

    int i=0;
    for(i=0; i<sectorsNeeded; i++)
    {
        int start = i == 0 ? startByte : 0; // if first sector
        int end = i == sectorsNeeded - 1 ? endByte : SECTOR_SIZE; // last sector

        /* If sector is only partially rewritten we need to read it first, edit it, 
        and rewrite everything as only full sectors can be written to disk
        . For now, read everything, edit, rewrite */

        /* Read */
        char * flpBuf = floppy_readsector(firstSector+i, fs->drive_id);

        /* overwrite in flpBuf */
        memcpy(buffer + offset, flpBuf + start , end - start);

        /* rewrite */
        floppy_writesector(flpBuf, firstSector+i, fs->drive_id);

        offset += end - start;
    }
    return offset;
}                                        

FileSystem * getFirstFileSystem()
{
    return firstFileSystem;
}

void filesystem_LoadFileList(FileSystem * fs)
{
    int sectors = filesystem_getFileSectors(11, fs);
    fs->root.data = (char*) sys_alloc(sectors * SECTOR_SIZE);
    fs->root.length = sectors * SECTOR_SIZE;
    fs->root.firstSector = 11;
    filesystem_readbytesByFirstSector(11, 0, sectors*SECTOR_SIZE, fs->root.data, fs);
}

FileList filesystem_loadSubFileList(FileSystem * fs, FileList fl, 
    char * subFileListName)
{
    FileList subfl;
    /* First sector is returned raw */
    int firstSector = filesystem_findFirstSector(subFileListName, fl) & 0x7FFF;

    if(firstSector == 0)
    {
        subfl.length = 0;
        return subfl;
    }
    

    int sectors = filesystem_getFileSectors(firstSector & 0x7FFF, fs);
    subfl.data = (char*) sys_alloc(sectors * SECTOR_SIZE);
    filesystem_readbytesByFirstSector(firstSector, 0, sectors*SECTOR_SIZE, subfl.data, fs);
    
    subfl.length = sectors * SECTOR_SIZE;
    subfl.firstSector = firstSector;
    return subfl;
}

FileSystem * getFileSystemByDriveId(int id)
{
    FileSystem * currentFileSystem = firstFileSystem;
    while(currentFileSystem != (void*)0)
    {
        if(currentFileSystem->drive_id == id)
            return currentFileSystem;
        currentFileSystem = currentFileSystem->next;
    }
    prints("Warning : unable to find fs for drive ");printi(id);prints("\n");
    return (void*)0;
}

void filesystem_list(FileSystem * fs, FileList fl)
{
    // entry : "NAME"\0 SIZE(16bits) FIRSTSECTOR(16bits)
    // prints("filename : Size : first Sector\n");

    int pos = 0;
    while(pos < fl.length)// make sure to parse all data, in case files have been deleted
    {
        if(fl.data[pos] != 0) // new entry
        {
            // output format = NAME : SIZE : FIRSTSECTOR
            prints(&(fl.data[pos]));prints(" ");
            pos += strlen(&(fl.data[pos])) + 1;
            unsigned short size = *((unsigned short*)&(fl.data[pos]));
            unsigned short firstSector = *((unsigned short*)&(fl.data[pos+2]));
            if(firstSector & 0x8000)
                prints(" (d) ");
            else
                prints(" (f) ");
            unsigned short sectorCount = filesystem_getFileSectors(firstSector & 0x7FFF, fs);
            printi((int)size);prints(" ");printi((int)firstSector & 0x7FFF);
            prints(" ");
            printi(sectorCount);
            prints("\n");
            pos += 4;
        }
        else
        {
            pos++;
        }
    }
}

unsigned short filesystem_findFirstSector(char * filename, FileList fl)
{
    int pos = 0;
    while(pos < fl.length)
    {
        if(fl.data[pos] != 0)
        {
            if(strcmp(&(fl.data[pos]), filename) == 0) // entry found
            {
                // retrieve the corresponding first sector
                pos += strlen(&(fl.data[pos])) + 3;
                unsigned short firstSector = *((unsigned short*)(&(fl.data[pos])));
                return firstSector;
            }
            else
            {
                pos += strlen(&(fl.data[pos])) + 5;
            }
        }
        else
        {
            pos++;
        }
    }
    return 0;// this means nothing found
}

unsigned short filesystem_getLastSector(unsigned short firstSector, FileSystem * fs)
{
    unsigned short currentSector = firstSector;
    while(filesystem_getNextSector(currentSector, fs))
        currentSector = filesystem_getNextSector(currentSector, fs);
    return currentSector;
}

unsigned short filesystem_findEmptySector(unsigned short first, FileSystem * fs)
{
    // scan through the FAT looking for the first sector marked as unused
    unsigned short i = first;
    while((*((unsigned short*)((int) fs->FATpointer + 2*i))) & 0x8000)
        i++;
    return i;
}

int filesystem_appendSectors(unsigned short first, int count, int rewrite, FileSystem * fs)
{
    unsigned short currentSector = filesystem_getLastSector(first, fs);
    int i;
    unsigned short nextSector = 0;
    for(i=0; i<count; i++)
    {
        nextSector = filesystem_findEmptySector(nextSector, fs);
        filesystem_setNextSector(currentSector, nextSector, fs);
        currentSector = nextSector;
    }

    /* Rewrite FAT to disk here */
    if(rewrite)
    {
        // rewrite the whole FAT into drive
        filesystem_writebytesByFirstSector(1, 0, 5120, fs->FATpointer, fs);
    }

    return 0;
}


void filesystem_setNextSector(unsigned short start, unsigned short next, FileSystem * fs)
{
    /* Write only value in start but set used flag in next */
    FAT_ENTRYAT(fs->FATpointer, start) = next | 0x8000;
    FAT_ENTRYAT(fs->FATpointer, next) |= 0x8000;

}

int filesystem_appendBytes(FileSystem * fs, FileList fl,
    char * filename, int bytesToAppend)
{
    /* If we truly get access to the file (no more subdir), then perform bytes appending.
    No more subdirs if the filename contains at most one '/' Otherwise, recursively load the filelists*/
    if(chrCount(filename, '/') <= 1){
        filename = (char*) (filename + 1);
        int firstSector = filesystem_findFirstSector(filename, fl);
        int sectors = filesystem_getFileSectors(firstSector, fs);
        int length = filesystem_getBytes(fl, filename);

        int sectorsToAppend = REQ_SECTORS(length + bytesToAppend) - REQ_SECTORS(length);


        // prints("Updating : ");prints(filename);prints("\n");
        /* Actually append the sectors if needed */
        prints("length :");printi(length);prints("\n");
        prints("sectors :");printi(sectors);prints("\n");
        prints("bytesToAppend :");printi(bytesToAppend);prints("\n");
        prints("sectors to append :");printi(sectorsToAppend);prints("\n");
        filesystem_appendSectors(firstSector, sectorsToAppend, 1, fs);
        /* Update the filelist */
        filesystem_setBytes(fl, filename, length + bytesToAppend);
        // rewrite FAT if modified
        /// TODO HERE
        // prints("Rewriting filetable starting at ");printi(fl.firstSector);prints("\n");
        filesystem_writebytesByFirstSector(fl.firstSector, 0, fl.length, fl.data, fs);
    }
    else
    {
        /* Strip of first part of filename */
        int tmpIndex = strIndexOf((char*) (filename+1), '/') + 1;
        filename[tmpIndex] = 0; // cut everything after 2nd '/'

        /* Then load corresponding subfilelist */
        FileList subfl;
        subfl = filesystem_loadSubFileList(fs, fl, (char*) (filename+1));
        if(subfl.length == 0)
        {
            prints("Not found\n");
            return -1;
        }

        /* Now use only the remaining part of the subListName */
        filename = (char*) (filename + tmpIndex);
        filename[0] = '/';
        int res = filesystem_appendBytes(fs, subfl, filename, bytesToAppend);

        filesystem_fileListFreeBuffer(subfl);
        return res;
    } 
}

int filesystem_getBytes(FileList fl, char* filename)
{
    // entry : "NAME"\0 SIZE(16bits) FIRSTSECTOR(16bits)

    int pos = 0;
    while(pos < fl.length)// make sure to parse all data, in case files have been deleted
    {
        if(fl.data[pos] != 0) // new entry
        {
            // output format = NAME : SIZE : FIRSTSECTOR
            // prints(&(fileListData[pos]));prints(" ");
            if(strcmp((char*) (fl.data + pos), filename) == 0)
            {
                pos += strlen(&(fl.data[pos])) + 1;
                unsigned short size = *((unsigned short*)&(fl.data[pos]));
                return size;
            }
            pos += strlen(&(fl.data[pos])) + 5;
        }
        else
        {
            pos++;
        }
    }
    return -1;
}

int filesystem_setBytes(FileList fl, char* filename, 
    int newSize)
{
    // entry : "NAME"\0 SIZE(16bits) FIRSTSECTOR(16bits)

    int pos = 0;
    while(pos < fl.length)// make sure to parse all data, in case files have been deleted
    {
        if(fl.data[pos] != 0) // new entry
        {
            // output format = NAME : SIZE : FIRSTSECTOR
            // prints(&(fileListData[pos]));prints(" ");
            if(strcmp((char*) ((fl.data) + pos), filename) == 0)
            {
                pos += strlen(&(fl.data[pos])) + 1;
                *((unsigned short*)&(fl.data[pos])) = (unsigned short) newSize;
                return 0;

                // rewrite the whole table here

            }
            pos += strlen(&(fl.data[pos])) + 5;
        }
        else
        {
            pos++;
        }
    }
    return -1;
}

int filesystem_subList(FileSystem * fs, FileList fl, char * subListName)
{
    if(strcmp(subListName, "/") == 0)
    {

        // already in the right directory, display and return
        filesystem_list(fs, fl);
        return 0;
    } 
    else
    {

        /* Strip of first part of subListName */
        int tmpIndex = strIndexOf((char*) (subListName+1), '/') + 1;
        subListName[tmpIndex] = 0; // cut everything after 2nd '/'
        

        /* Then load corresponding subfilelist */
        FileList subfl;
        subfl = filesystem_loadSubFileList(fs, fl, (char*) (subListName+1));
        if(subfl.length == 0)
        {
            prints("Not found\n");
            return -1;
        }

        /* Now use only the remaining part of the subListName */
        subListName = (char*) (subListName + tmpIndex);
        subListName[0] = '/';
        int res = filesystem_subList(fs, subfl, subListName);

        filesystem_fileListFreeBuffer(subfl);
        return res;
    }
}

void filesystem_fileListFreeBuffer(FileList fl)
{
    sys_free(fl.data);
}

int filesystems_driveList(char * fullPath)
{
    int driveId = (int) (fullPath[0] - '0');
    // prints("Listing ");prints(fullPath);prints("\n");

    FileSystem * current =  getFirstFileSystem();
    while(driveId > 0)
    {
        current = current->next;
        driveId--;
    }

    // read from subFullPath from requested drive
    filesystem_subList(current, current->root, (char*) fullPath + 1);
}
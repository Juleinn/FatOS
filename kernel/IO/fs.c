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

FileSystem * getFirstFileSystem()
{
    return firstFileSystem;
}

void filesystem_LoadFileList(FileSystem * fs)
{
    // read the whole filelist file into memory
    // probably scales poorly but can be solved by adding folders
    // filelist starts in sector 11
    int sectors = filesystem_getFileSectors(11, fs);
    fs->fileList = (char*) sys_alloc(sectors * SECTOR_SIZE);
    fs->fileListSize = sectors * SECTOR_SIZE;
    filesystem_readbytesByFirstSector(11, 0, sectors*SECTOR_SIZE, fs->fileList, fs);
    // prints("Loaded file list for drive ");printi(fs->drive_id); prints("\n");
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

void filesystem_list(char* fileListData, int length)
{
    // entry : "NAME"\0 SIZE(16bits) FIRSTSECTOR(16bits)
    prints("filename : Size : first Sector\n");

    int pos = 0;
    while(pos < length)// make sure to parse all data, in case files have been deleted
    {
        if(fileListData[pos] != 0) // new entry
        {
            // output format = NAME : SIZE : FIRSTSECTOR
            prints(&(fileListData[pos]));prints(" ");
            pos += strlen(&fileListData[pos]) + 1;
            unsigned short size = *((unsigned short*)&fileListData[pos]);
            unsigned short firstSector = *((unsigned short*)&fileListData[pos+2]);
            printi((int)size);prints(" ");printi((int)firstSector);prints("\n");
            pos += 4;
        }
        else
        {
            pos++;
        }
    }
}

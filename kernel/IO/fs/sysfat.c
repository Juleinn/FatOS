#include "sysfat.h"

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

unsigned short filesystem_getLastSector(unsigned short firstSector, FileSystem * fs)
{
    unsigned short currentSector = firstSector;
    while(filesystem_getNextSector(currentSector, fs))
        currentSector = filesystem_getNextSector(currentSector, fs);
    return currentSector;
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
/**
@file sysfat.h
@brief contains functions for handling the File Allocation Table for the filesystem
*/
#ifndef SYSFAT_H_
#define SYSFAT_H_

#include "fs.h"

/**
@brief Gets sector after a given sector in the FAT 
@param sector the current sector
@param fs the filesystem to get the FAT from
@return the next sector's id in LBA, 0 if no next sector */
unsigned short filesystem_getNextSector(unsigned short sector, FileSystem * fs);

/**
@brief Gets sector after a given sector in the FAT, without striping the 'used' flag (highest bit)
@param sector the current sector
@param fs the filesystem to get the FAT from
@return the next sector's id in LBA, 0 if no next sector, with the used flag not striped */
unsigned short filesystem_getNextSectorRaw(unsigned short sector, FileSystem * fs);

/**
@brief gets the Nth sector after a given sector in FAT. Indexing starts at 0.
@param firstSector the firstSector to seek from (not necessarely the first sector of the file)
@param n the number of sectors to skip (n=0 returns the firstSector)
@param fs the filesystem to get the FAT from
@return the id of the Nth sector
*/
unsigned short filesystem_getNthSector(unsigned short firstSector, int n, FileSystem * fs);

/**
@brief gets the last sector of a chain of sectors in the FAT
@param first sector any sector in the chain to start with
@param fs the filesystem to get the FAT from
@return the id of the last sector in the chain
*/
unsigned short filesystem_getLastSector(unsigned short firstSector, FileSystem * fs);

/**
@brief computes number of sector a file actual uses (based on FAT, not on advertised size).
Based on the first sector.
Reads it from FAT loaded into memory. 
@param firstector the firstsector of the file. If the sector is not the first one it 
will return the number of sectors until the end of the file
@param fs the filesystem to read from
@return the id of the last sector of the file
*/
unsigned short filesystem_getFileSectors(unsigned short firstSector, FileSystem * fs);

#endif
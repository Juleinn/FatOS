/** @file sysdir.c
 *  @author Anton CLAES
 *  @date 2017
    @brief Contains filesystem functions for handling directories
 */

#include "sysdir.h"

void filesystem_LoadRootFileList(FileSystem * fs)
{
    int sectors = filesystem_getFileSectors(11, fs);
    fs->root.data = (char*) sys_alloc(sectors * SECTOR_SIZE);
    fs->root.length = sectors * SECTOR_SIZE;
    fs->root.firstSector = 11;
    filesystem_readbytesByFirstSector(11, 0, sectors*SECTOR_SIZE, fs->root.data, fs);
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

FileList filesystem_loadSubFileListByName(FileSystem * fs, FileList fl, 
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
        subfl = filesystem_loadSubFileListByName(fs, fl, (char*) (subListName+1));
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

FileList filesystem_LoadRootFileListByPath(char * path)
{
    int driveId = (int) (path[0] - '0');
    // prints("Listing ");prints(fullPath);prints("\n");

    FileSystem * current =  getFirstFileSystem();
    while(driveId > 0)
    {
        current = current->next;
        driveId--;
    }

    // read from subFullPath from requested drive
    return filesystem_LoadSubFileList(current, current->root, (char*) path + 1, 0);
}

FileList filesystem_LoadSubFileList(FileSystem * fs, FileList fl, char * subListPath, int level)
{
    if(strcmp(subListPath, "/") == 0)
    {

        // already in the right directory, return
        return fl;
    } 
    else
    {

        /* Strip of first part of subListName */
        int tmpIndex = strIndexOf((char*) (subListPath+1), '/') + 1;
        subListPath[tmpIndex] = 0; // cut everything after 2nd '/'
        

        /* Then load corresponding subfilelist */
        FileList subfl;
        subfl = filesystem_loadSubFileListByName(fs, fl, (char*) (subListPath+1));
        if(subfl.length == 0)
        {
            prints("Not found\n");
            return subfl;
        }

        /* Now use only the remaining part of the subListName */
        subListPath = (char*) (subListPath + tmpIndex);
        subListPath[0] = '/';
        FileList res = filesystem_LoadSubFileList(fs, subfl, subListPath, level + 1);
        /* free only intermediate file lists (not first (root) and last one */
        if(level != 0 && strcmp(subListPath, "/") == 0)
            filesystem_fileListFreeBuffer(subfl);
        return res;
    }
}

int filesystem_createfile(const char * fullpath, int dirFlag)
{
    /*  - find filelist to edit 
        - find spot in filelist
        - while !spot -> extend filelist
        - find empty sector
        - write FAT
        - write filelist                        */
    FileList fl = filesystem_LoadRootFileListByPath(fullpath);
    char * filename = fullpath + sys_getPathFileIndex(fullpath); 
    int spot;
    // while(spot = filesystem_findemptyFilelistSpot() == -1)
    // {
    //     /* expand the */
    // }


}

int filesystem_findemptyFilelistSpot(FileList fl, int size)
{
    /* Start at beginning, skip entries, */
    int index = 0;
    int subIndex = 0;
    while(index < fl.length)
    {
        if(fl.data[index] == 0)
        {
            /* count the amount of empty bytes */
            while(fl.data[index+subIndex] == 0)
                subIndex++;
            if(subIndex > size)
                return index;
        }
        else
        {
            subIndex = 0;
            index += strlen(&fl.data[index]) + 5;
        }

        index++;
    }
    return -1;
}

int filesystem_cutparentdirectory(char * filename)
{
    if(strlen(filename) == 2)
    {
        // root directory
        return 1;
    }
    else
    {
        filename[strlen(filename)] = '\0'; // cut last '/' 
        filename[sys_getPathFileIndex(filename)-1] = '\0';
        filename[sys_getPathFileIndex(filename)] = '\0';
        if(strlen(filename) == 2)
            return 1;
        else
            return 0;
    }

}

int filessytem_addFilelistBytes(const char * path, int bytes)
{
    if(filesystem_cutparentdirectory(path) == 1)
    {
        prints(path);prints("\n");
        /* load filelist from filelist.bin */
        strcpy("filelist2aaa.bin", path+2);

        prints(path);prints("\n");

        FileList fl = filesystem_LoadRootFileListByPath(path);
        
        FileSystem * fs = getFileSystemByDriveId(path[0] - '0');
        filesystem_appendBytes(fs, fs->root, "/subdir/subfile1.bin", bytes);

        return 0;
    }
    else
    {
        return 0;
    }
}
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

/* 10 * 512 bytes */
uint16_t FAT[5*512] = {0};

int findFreeSector()
{
  int i = 0;
  while(FAT[i] & (uint16_t) 0x8000)
  {
    i++;
    printf("FAT[%d]=%d\n",i,FAT[i]);
  }
  return i;
}

int addFileToFat(uint16_t size)
{
  int firstSector = findFreeSector();
  int currentSector = firstSector;
  int sectors = size % 512 == 0 ? size / 512 : (size / 512) + 1;
  FAT[firstSector] |= (uint16_t) 0x8000;
  while(sectors - 1){
    int nextSector = findFreeSector();
    printf("nextSector = %d\n", nextSector);
    FAT[currentSector] = (uint16_t) nextSector | 0x8000;
    currentSector = nextSector;
    FAT[nextSector] |= (uint16_t) 0x8000;
    sectors--;
  }
}


int main()
{
  FILE * table = fopen("FATandList.bin", "rb+");
  int cont = 1;
  char filename[50];
  char buffer[50];
  uint16_t size;
  uint16_t firstSector;
  int listPos = 512*10;
  fseek(table, 5120, SEEK_SET);
  while(cont)
  {
    printf("Please give in filename : ");
    fgets(filename, 50, stdin);
    filename[strlen(filename)-1] = '\0';
    printf("Size in Bytes : ");
    fgets(buffer, 50, stdin);
    buffer[strlen(buffer)-1] = '\0';
    size = (uint16_t) atoi(buffer);

    firstSector = addFileToFat(size);

    // write to filelist
    fprintf(table, "%s", filename);
    fseek(table, 1, SEEK_CUR);

    fwrite((char*)&size, 2,1,table);
    fwrite((char*)&firstSector, 2,1,table);
    listPos = ftell(table);

    printf("Continue (y/n)");
    fgets(buffer, 50, stdin);
    buffer[strlen(buffer)-1] = '\0';
    if(buffer[0] != 'y')
      cont = 0;
  }
  fseek(table, 0, SEEK_SET);
  fwrite(FAT, 5120, 1, table);
  fclose(table);
}

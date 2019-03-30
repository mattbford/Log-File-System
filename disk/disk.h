#define DISK_H_
#include<stdio.h>
#include<stdlib.h>

void writeBlock(FILE* disk, int block, char* data);
void readBlock(FILE* disk, int block, char* buff);
void createDisk(FILE* disk);
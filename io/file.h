#define FILE_H_
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../disk/disk.h"


char* createINode(FILE* disk, char* type, int* block_addr, int INode_Num, int indirect);
int findFreeBlock(FILE* disk);
int findFreeInode(FILE* disk, int INode_Addr);
void findINodeAddr(FILE* disk, char* Addr_Buffer, int INode_Num);
int findDirectoryAddr(FILE* disk, char* path);
char* readFile(FILE* disk, char* file_name, char* path);
int createFile(FILE* disk, char* type, char* file, char* name, char* path);
int writeToFile(FILE* disk, char* data, char* file_name, char* path);
int deleteFile(FILE* disk, char* file_name, char* path);
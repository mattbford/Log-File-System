#define FILE_H_
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../disk/disk.h"

char* readFile(FILE* disk, char* file_name);
void createFile(FILE* disk, char* type, char* file, char* name, char* path);
void writeToFile(FILE* disk, char* data, char* file_name);
void deleteFile(FILE* disk, char* data, char* file_name);
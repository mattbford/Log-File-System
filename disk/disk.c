#include <stdio.h>
#include <stdlib.h>

const int BLOCK_SIZE = 512;
const int NUM_INODES = 4096;
const int NUM_BLOCKS = 4096;

void writeBlock(FILE* disk, int block, char* data) {
    fseek(disk, block * BLOCK_SIZE, SEEK_SET);
    fwrite(data, BLOCK_SIZE, 1, disk);
}

void readBlock(FILE* disk, int block, char* buff) {
    fseek(disk, block * BLOCK_SIZE, SEEK_SET);
    fread(buff, BLOCK_SIZE, 1, disk);
}

void createDisk(FILE* disk) {
    char* init = calloc(BLOCK_SIZE*NUM_BLOCKS, 1);
    fwrite(init, BLOCK_SIZE*NUM_BLOCKS, 1, disk);

    // Create Superblock 1111 - magic number
    char* superInit = "123440964096";
    writeBlock(disk, 0, superInit);
    // Create Free Block Vector takes into account root dir at block 10
    char freeBlock[512] = { 0b00000000, 0b00011111, [2 ... 511] = 0b11111111 };       
    writeBlock(disk, 1, freeBlock);

    free(init);
}

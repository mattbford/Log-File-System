#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../disk/disk.h"

const int INODE_SIZE = 32;
const int debug = 1;
const int BLOCK_SIZE;
const int NUM_BLOCKS;
int NUM_INODES = 0;

char* createINode(FILE* file, char* type, int block) {
    char* INode = malloc(INODE_SIZE);

    //Convert INODE number to 4 byte string
    INode[0] = (NUM_INODES >> 24) & 0xFF;
    INode[1] = (NUM_INODES >> 16) & 0xFF;
    INode[2] = (NUM_INODES >> 8) & 0xFF;
    INode[3] = NUM_INODES & 0xFF;

    //add type
    if(strlen(type) == 4) {
        // type 1111 - directory 0000 - flat file
        strcat(INode, type);
    }
    else {
        printf("type invalid");
        return INode; //TODO ERROR
    }
    

    return INode;
}

void createFile(FILE* disk) {
    //find free block
    char* buffer = malloc(BLOCK_SIZE);
    readBlock(disk, 1, buffer);
    int block_addr = -1;
    int i;
    int bits;

    //get the first 1 bit in superblock
    for(i = 0; i < BLOCK_SIZE; i++) {
        char byte = buffer[i];
        for(bits = 0; bits < 8; bits++) {
            if((byte & 0x80) > 0) {
                char temp = 0x80;
                temp >>= bits;
                buffer[i] &= ~temp; //find taken bit and set to 0
                writeBlock(disk, 1, buffer); //write taken in superblock
                block_addr = (i * 8) + bits; //set block of address
                break;
            }
            byte = byte << 1;
        }
        if(block_addr != -1) {            
            break;
        }
    }
    if(block_addr == -1) {
        printf("DISK FULL\n");
        return;
    }
    free(buffer);

    if(debug) {
        printf("address: %d\n", block_addr);
    }

    //create INODE

}

void writeToFile(FILE* disk, char* data, char* INode) {
    //
}

void readFile(FILE* disk, char* buffer, char* INode) {

}

int main (int argc, char* argv[]) {
    FILE* disk = fopen("../disk/vdisk", "r+b");

    //if disk needs to be created
    if(argc > 1) {
        if(strcmp(argv[1], "init") == 0) {
            createDisk(disk);
        }
    }
    
    createFile(disk);
    
    fclose(disk);
}

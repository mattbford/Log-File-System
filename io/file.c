#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../disk/disk.h"

//TO_HEX code from: https://stackoverflow.com/questions/4085612/converting-an-int-to-a-2-byte-hex-value-in-c
#define TO_HEX(i) (i <= 9 ? '0' + i : 'A' - 10 + i)

const int INODE_SIZE = 32;
const int debug = 1;
const int NUM_INODES;
const int BLOCK_SIZE;
const int NUM_BLOCKS;
//int NUM_INODES = 0;

//int indirect: 0 - not an indirect, 1 - single indirect, 2 - double indirect
char* createINode(FILE* disk, char* type, int* block_addr, int INode_Num, int indirect) {
    char* INode = (char*)calloc(INODE_SIZE * sizeof(char),1);

    //Convert INODE number to 4 byte string
    char temp[5];
    temp[0] = ((INode_Num / 1000) % 10) + '0';
    temp[1] = ((INode_Num / 100) % 10) + '0';
    temp[2] = ((INode_Num / 10) % 10) + '0';
    temp[3] = (INode_Num % 10) + '0';
    temp[4] = '\0';
    strncat(INode, temp, 4);

    //NUM_INODES++;

    //add type
    if(strlen(type) == 4) {
        // type 1111 - directory 0000 - flat file
        strncat(INode, type, 4);
    }
    else {
        printf("type invalid");
        return INode; //TODO ERROR
    }

    //put first 10 block addresses into INODE as integers; 0000 means none
    int i;
    char temp2[21];
    temp2[20] = '\0';
    int len_block_addr = sizeof(block_addr) / sizeof(int);
    for(i = indirect * 10; i < 10 * (indirect + 1); i++) {
        int temp_addr = ((i - (indirect * 10))) * 2;
        if(i > len_block_addr) {
            temp2[temp_addr] = '0';
            temp2[temp_addr+1] = '0';
        }else {
            int j = block_addr[i];
            temp2[temp_addr] = TO_HEX(((j & 0xF0) >> 4));
            temp2[temp_addr + 1] = TO_HEX((j & 0x0F));
        }        
    }
    strncat(INode, temp2, 20);

    //build indirects
    /*if(indirect == 0 || indirect == 2) {
        char* single_indirect_INode;
        
        if(len_block_addr > 10) {
            int addr = findFreeBlock(disk);
            single_indirect_INode = createINode(disk, type, block_addr, 1);
            
        }
        if(indirect == 0) {
            char* double_indirect_INode;
            if(len_block_addr > 20) {
                int addr = findFreeBlock(disk);
                double_indirect_INode = createINode(disk, type, block_addr, 2);
            }
            else {
                
            }
        }    
    }*/

    strncat(INode, "0000", 4);

    return INode;
}

//grabs first available block
int findFreeBlock(FILE* disk) {
    char* buffer = malloc(BLOCK_SIZE);
    readBlock(disk, 1, buffer);
    int i;
    int bits;
    int block_addr = 0;

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
        if(block_addr != 0) {            
            break;
        }
    }
    free(buffer);
    return block_addr;
}

int findFreeInode(FILE* disk, int INode_Addr) {
    int i, j;
    int counter = 0;
    char temp[3];
    char* buffer;

    temp[0] = TO_HEX(((INode_Addr & 0xF0) >> 4));
    temp[1] = TO_HEX((INode_Addr & 0x0F));
    temp[2] = '\0';

    for(i = 2; i < 10; i++) {
        buffer = malloc(BLOCK_SIZE);
        readBlock(disk, i, buffer);
        for(j = 0; j < BLOCK_SIZE-1; j+=2) {
            if(buffer[j] == 0 && buffer[j+1] == 0) {
                buffer[j] = temp[0];
                buffer[j+1] = temp[1];
                writeBlock(disk, i, buffer);
                free(buffer);
                return counter;
            }
            counter++;
        }
        free(buffer);
    }
    return -1;
}

void createFile(FILE* disk, char* type, char* file) {
    //find free block
    //find number of blocks needed to store file rounding up
    int blocks_req = (strlen(file) + BLOCK_SIZE - 1)/ BLOCK_SIZE;
    int* block_addr = calloc(blocks_req, 1);
    int i;
    int j;

    //get the first 1 bit in superblock
    for(j = 0; j < blocks_req; j++) {
        block_addr[j] = findFreeBlock(disk);
        if(block_addr[j] == 0) {
            printf("FIND BLOCK FAILED, DISK MIGHT BE FULL\n");
            return;
        }
    }

    if(debug) {
        for(i = 0; i < blocks_req; i++) {
            printf("address: %d\n", block_addr[i]);
        }        
    }

    //create INODE
    int INode_Addr = findFreeBlock(disk);
    int INode_Num = findFreeInode(disk, INode_Addr);
    if(INode_Num != -1) {
        char* INode = createINode(disk, type, block_addr, INode_Num, 0);
    }
    else {
        printf("Failed to Create File, INode Map full");
        return;
    }    

    if(debug == 1) {
        printf("INode: %s\n", INode);
    }
    
    if(INode_Addr != 0) {
        writeBlock(disk, INode_Addr, INode);
    }
    else {
        printf("Failed to create file, Disk Full");
    }

    //read blocks 2-9 and find location to write mapping
    //INodes take 2 bytes
    //int INode_block = (NUM_INODES / (BLOCK_SIZE / 2)) + 2;
    //write to appropriate directory - root for now
}

void writeToFile(FILE* disk, char* data, char* file_name) {
    //
}

void readFile(FILE* disk, char* buffer, char* file_name) {

}

int main (int argc, char* argv[]) {
    FILE* disk = fopen("../disk/vdisk", "r+b");

    //if disk needs to be created
    if(argc > 1) {
        if(strcmp(argv[1], "init") == 0) {
            createDisk(disk);
        }
    }
    char* file_con = "Gt85LeGhaEs7iGp7khXXrMMk08KFNBJhVroxE42rv6uEHSUBEjy9OHLhdj0lMFyHywAhJgz0XLky9PDUDFrPin7i6W5wAlQ6V53fUExBNkK9VNzFhgwG3ajig9ph9FhhCIeeA5UmsFD6Qnt8zREmAWFGO7qCJUwcCfrO8plpapGTC7apP2lGiQkj1sypu6WRcJFNidIcNTvDu7nJQJngUr9j7mye81iCQP1OnIunkV7Ho8rLyC5ZOPx3y9oY0XYtzIQkfRdFRwIHwVkXBSAq0dw8JUB8if6OAyC5bIEZiqnku7MwMuEo5a2k9Im3STJRzNwwsPjiiT9470DWqRVvMVBYnQDnciD0iIcdoPJhr0phzAhSCfbrEKYbeAD64UTBeXCRowwcrWKaO95GFI0r0pyCo6arOI3BgXVOH7mVYvMRQ7oF8R10Vauv7iwuCxJ4fE9gXwIG80uLAw35NIUz1BC39yobFehFfUMc05cBirZjWN6HcOh7jzOuPUwfDs4GgGR5IOWmCmE9C1Cr7ryCy6D6coazAE1Jr8IzmKkBIG7yyl5n42baolbQhO0bg7IZ9tg74h2ZrSAAmXhYNSM8NCSiRoD398r3A74ubVKFmdy2u7S3SrVWPpw0ZO3C8w5kGXfrAcGRXK4rYD1YwRkgDRHhI0OxQm33NxSzVqWQrUKQQnNE9x9Sxp6bLtzozy6Jl2Bjp4601mrDr9YF4IvtsfmGjYSekLqWDKzoR9QdPgUyV4CA4UoiS1AnZpDGDGQmwVPPl9O3vNsJ4zl9xs5txQ3xWuc2ZOtJJCymHvBeAiz7uy5WgjND1EZ1HbCvpScilubojqX89EqFllK76bnDK1rearuCZRWuEhdEkyzqEg2Ri3Inf5xT4V1bhZ6iKsPOT3K5RpKVGmpf43VFqZgmDCCMShPstalFVkYwdJMbaxctELcuX7IuzoGzN3d9xVeJFLiMTdHjeggz185NTaIJGsFqQON5CWKMdIdmkuymxJv0ICMCmi6LEGcw3hgBkbNCrQBGHeICHdQoRvjuX5HvYTkjIDva8lZz1SZOANdUF6kkGZ07HXWm9114DHelZSf2YyDev95YFZPC1xO1xWKgX0JPdIV5xjYGIj1BzdBfcSgU00I8SyvzfJRHPdRDv6dCu1Gr7CW0ZJtpjDNkwpHbRao0laJrOiNhURACtZy8Us3cUxthw7J5g9YX3DTpCrpYwjaQOOLGWfZaTYwA0ixPCypAzjlqMqF6FOBr7vHnbifYijC76eZoezSsf28zejLahY29ry3mTit3CM3FPD4hVVqvaP6hU5JfDALzh1ATGfvLutBAD0VJp7UrTtSTazeALfnN7zOwN5M475sNYhcbhUsIi5e7bPXQiLmJAj4Gt0dFCJ18kNvfNADRZaOyNg3jx4Wqks6vQcL9qSH6a9tADP9KXQLqZagXkivtus4UGzvHyTHp6yPZ1kHnVzRXW3XvNMXf7wSdrCB9jYWiab18i9BXbduS";
    printf("%lu\n", strlen(file_con));
    createFile(disk, "1111", file_con);
    createFile(disk, "0000", file_con);
    fclose(disk);
}

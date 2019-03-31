#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../disk/disk.h"

const int INODE_SIZE = 32;
const int debug = 1;
const int BLOCK_SIZE;
const int NUM_BLOCKS;
int NUM_INODES = 0;

char* createINode(FILE* file, char* type, int* block_addr) {
    char * INode = (char*)malloc(INODE_SIZE);

    //Convert INODE number to 4 byte string
    char temp[4];
    temp[0] = (NUM_INODES >> 24) & 0xFF + '0';
    temp[1] = (NUM_INODES >> 16) & 0xFF + '0';
    temp[2] = (NUM_INODES >> 8) & 0xFF + '0';
    temp[3] = NUM_INODES & 0xFF + '0';
    strncat(INode, temp, 4);

    //add type
    if(strlen(type) == 4) {
        // type 1111 - directory 0000 - flat file
        strncat(INode, type, 4);
    }
    else {
        printf("type invalid");
        return INode; //TODO ERROR
    }

    //put first 10 block addresses into INODE
    int i;
    for(i = 0; i < sizeof(block_addr); i++) {
        INode[8+i] = (block_addr[i] >> 8) & 0xFF + '0';
        INode[9+i] = block_addr[i] & 0xFF + '0';
    }

    return INode;
}

void createFile(FILE* disk, char* type, char* file) {
    //find free block
    //find number of blocks needed to store file rounding up
    int blocks_req = (strlen(file) + BLOCK_SIZE - 1)/ BLOCK_SIZE;
    char* buffer = malloc(BLOCK_SIZE);
    readBlock(disk, 1, buffer);
    int* block_addr = calloc(blocks_req, 1);
    int i;
    int j;
    int bits;

    //get the first 1 bit in superblock
    for(j = 0; j < blocks_req; j++) {
        for(i = 0; i < BLOCK_SIZE; i++) {
            char byte = buffer[i];
            for(bits = 0; bits < 8; bits++) {
                if((byte & 0x80) > 0) {
                    char temp = 0x80;
                    temp >>= bits;
                    buffer[i] &= ~temp; //find taken bit and set to 0
                    writeBlock(disk, 1, buffer); //write taken in superblock
                    block_addr[j] = (i * 8) + bits; //set block of address
                    break;
                }
                byte = byte << 1;
            }
            if(block_addr[j] != 0) {            
                break;
            }
        }
        if(block_addr[j] == 0) {
            printf("DISK FULL\n");
            return;
        }
    }    
    free(buffer);

    if(debug) {
        for(i = 0; i < blocks_req; i++) {
            printf("address: %d\n", block_addr[i]);
        }        
    }

    //create INODE
    char* INode = createINode(disk, type, block_addr);
    for(i = 0; i < INODE_SIZE; i++) {
        printf("%c", INode[i]);
    }
    //printf("INode: %s\n", INode);

    //read blocks 2-9 and find location to write mapping

    //write to appropriate directory - root for now


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
    char* file_con = "Gt85LeGhaEs7iGp7khXXrMMk08KFNBJhVroxE42rv6uEHSUBEjy9OHLhdj0lMFyHywAhJgz0XLky9PDUDFrPin7i6W5wAlQ6V53fUExBNkK9VNzFhgwG3ajig9ph9FhhCIeeA5UmsFD6Qnt8zREmAWFGO7qCJUwcCfrO8plpapGTC7apP2lGiQkj1sypu6WRcJFNidIcNTvDu7nJQJngUr9j7mye81iCQP1OnIunkV7Ho8rLyC5ZOPx3y9oY0XYtzIQkfRdFRwIHwVkXBSAq0dw8JUB8if6OAyC5bIEZiqnku7MwMuEo5a2k9Im3STJRzNwwsPjiiT9470DWqRVvMVBYnQDnciD0iIcdoPJhr0phzAhSCfbrEKYbeAD64UTBeXCRowwcrWKaO95GFI0r0pyCo6arOI3BgXVOH7mVYvMRQ7oF8R10Vauv7iwuCxJ4fE9gXwIG80uLAw35NIUz1BC39yobFehFfUMc05cBirZjWN6HcOh7jzOuPUwfDs4GgGR5IOWmCmE9C1Cr7ryCy6D6coazAE1Jr8IzmKkBIG7yyl5n42baolbQhO0bg7IZ9tg74h2ZrSAAmXhYNSM8NCSiRoD398r3A74ubVKFmdy2u7S3SrVWPpw0ZO3C8w5kGXfrAcGRXK4rYD1YwRkgDRHhI0OxQm33NxSzVqWQrUKQQnNE9x9Sxp6bLtzozy6Jl2Bjp4601mrDr9YF4IvtsfmGjYSekLqWDKzoR9QdPgUyV4CA4UoiS1AnZpDGDGQmwVPPl9O3vNsJ4zl9xs5txQ3xWuc2ZOtJJCymHvBeAiz7uy5WgjND1EZ1HbCvpScilubojqX89EqFllK76bnDK1rearuCZRWuEhdEkyzqEg2Ri3Inf5xT4V1bhZ6iKsPOT3K5RpKVGmpf43VFqZgmDCCMShPstalFVkYwdJMbaxctELcuX7IuzoGzN3d9xVeJFLiMTdHjeggz185NTaIJGsFqQON5CWKMdIdmkuymxJv0ICMCmi6LEGcw3hgBkbNCrQBGHeICHdQoRvjuX5HvYTkjIDva8lZz1SZOANdUF6kkGZ07HXWm9114DHelZSf2YyDev95YFZPC1xO1xWKgX0JPdIV5xjYGIj1BzdBfcSgU00I8SyvzfJRHPdRDv6dCu1Gr7CW0ZJtpjDNkwpHbRao0laJrOiNhURACtZy8Us3cUxthw7J5g9YX3DTpCrpYwjaQOOLGWfZaTYwA0ixPCypAzjlqMqF6FOBr7vHnbifYijC76eZoezSsf28zejLahY29ry3mTit3CM3FPD4hVVqvaP6hU5JfDALzh1ATGfvLutBAD0VJp7UrTtSTazeALfnN7zOwN5M475sNYhcbhUsIi5e7bPXQiLmJAj4Gt0dFCJ18kNvfNADRZaOyNg3jx4Wqks6vQcL9qSH6a9tADP9KXQLqZagXkivtus4UGzvHyTHp6yPZ1kHnVzRXW3XvNMXf7wSdrCB9jYWiab18i9BXbduS";
    printf("%lu\n", strlen(file_con));
    createFile(disk, "1111", file_con);
    fclose(disk);
}

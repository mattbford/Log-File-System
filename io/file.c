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

    //printf("FINDINODE: ADDR(HEX): %s\n", temp);

    for(i = 2; i < 10; i++) {
        buffer = malloc(BLOCK_SIZE);
        readBlock(disk, i, buffer);
        for(j = 0; j < BLOCK_SIZE-1; j+=2) {
            if(i != 2 || j > 2) {
                if(buffer[j] == 0 && buffer[j+1] == 0) {
                    buffer[j] = temp[0];
                    buffer[j+1] = temp[1];
                    writeBlock(disk, i, buffer);
                    free(buffer);
                    return counter;
                }
            }            
            counter++;
        }
        free(buffer);
    }
    return -1;
}

void findINodeAddr(FILE* disk, char* Addr_Buffer, int INode_Num) {
    char* buffer = malloc(BLOCK_SIZE);
    int i = INode_Num / (BLOCK_SIZE / 2); //get which block INODEaddr is in
    readBlock(disk, i + 2, buffer);
    INode_Num = (INode_Num * 2) - (i * BLOCK_SIZE); //get address (in hex)

    if(debug == 1) {
        printf("FINDINODEADDR: i: %d\n", i);
        printf("FINDINODEADDR: NUM: %d\n", INode_Num);  
    }
    
    Addr_Buffer[0] = buffer[INode_Num];
    Addr_Buffer[1] = buffer[INode_Num + 1];
    Addr_Buffer[2] = '\0';
    free(buffer);
}

int findDirectoryAddr(FILE* disk, char* path) {
    // break up path
    //finds proper directory
    char* dir = malloc(BLOCK_SIZE);
    readBlock(disk, 10, dir);
    int i;
    int j;
    int n = 0;
    int INode_Num;

    char temp_path[strlen(path)]; // to prevent altering of path
    strcpy(temp_path, path);
    char** steps = NULL;
    const char s[2] = "/";
    char* token = strtok(temp_path, s);
    
    while (token != NULL) {
        if(strcmp(token, "root") != 0) {
            steps = realloc(steps, sizeof(char*) * ++n);
            if(steps == 0) {
                printf("READ: Realloc Failed\n");
                exit (-1);
            }
            steps[n-1] = token;
        }
        token = strtok(NULL, s);
    }

    //run through directories
    for(j = 0; j < n; j++) {
        for(i = 0; i < BLOCK_SIZE; i+=32) {
            int z = 0;
            char* temp = malloc(strlen(steps[j]));
            while(z < strlen(steps[j])) {
                temp[z] = dir[i + z + 1];
                z++;
            }
            if(strncmp(temp, steps[j], strlen(steps[j])) == 0) {
                INode_Num = dir[i] - '0';
                free(temp);
                break;
            }
            free(temp);

            if(i == BLOCK_SIZE-32) {
                printf("FINDDIRADDR: directory: %s does not exist in current directory\n", steps[j]);
                free(dir);
                return -1;
            }
        }
        free(dir);

        //get inode address from inode number
        char INode_Addr[3];    
        findINodeAddr(disk, INode_Addr, INode_Num);
        int addr = (int)strtol(INode_Addr, NULL, 16);    //hex to int

        //get inode from inode address
        char* INode = malloc(BLOCK_SIZE);
        readBlock(disk, addr, INode);

        //get first block from inode (directories are only 1 block large)
        char temp[3] = { INode[8], INode[9], '\0' };
        int block_addr = (int)strtol(temp, NULL, 16);
        if(j == n-1) {
            printf("FINDDIRADDR: %d\n", block_addr);
            return block_addr;
        }
        else {
            dir = malloc(BLOCK_SIZE);
            readBlock(disk, block_addr, dir);
        }
    }
}

char* readFile(FILE* disk, char* file_name, char* path) {
    //finds proper directory
    char* dir = malloc(BLOCK_SIZE);
    int i;
    int j;
    int INode_Num;

    if(strcmp(path, "root") != 0) {
        int addr = findDirectoryAddr(disk, path);
        if(addr == -1) {
            printf("READ: failed %s does not exist\n", path);
            return '\0';
        }
        readBlock(disk, addr, dir);
    }
    else {        
        readBlock(disk, 10, dir);
    }

    //check all filenames in current directory and get INODE_NUM if it exists
    for(i = 0; i < BLOCK_SIZE; i+=32) {
        j = 0;
        char* temp = malloc(strlen(file_name));
        while(j < strlen(file_name)) {
            temp[j] = dir[i + j + 1];
            j++;
        }
        if(strncmp(temp, file_name, strlen(file_name)) == 0) {
            INode_Num = dir[i] - '0';
            free(temp);
            break;
        }
        free(temp);

        if(i == BLOCK_SIZE-32) {
            printf("READ: file: %s does not exist in current directory\n", file_name);
            free(dir);
            return '\0';
        }
    }

    //get Inode address from inode number
    free(dir);
    char INode_Addr[3];    
    findINodeAddr(disk, INode_Addr, INode_Num);
    int addr = (int)strtol(INode_Addr, NULL, 16);    //hex to int

    if(debug == 1) {
        printf("READ: INODE NUM: %d\n", INode_Num);      
        printf("READ: INODE ADDR: %s\n", INode_Addr);
    }

    //get inode from inode address
    char* INode = malloc(BLOCK_SIZE);
    readBlock(disk, addr, INode);
    int blocks[10] = { 0 };
    int num_blocks = 0;

    //get number of blocks and their addresses from INode bytes 8 - 27
    //TODO handle indirects
    for(i = 8; i < 27; i += 2) {
        if(INode[i] == '0' && INode[i+1] == '0') {
            break;
        }
        else {
            char temp[3] = { INode[i], INode[i+1], '\0' };
            blocks[(i-8)/2] = (int)strtol(temp, NULL, 16);
            num_blocks++;
        }
    }
    
    if(debug == 1) {
        /*for(i = 0; i < 10; i++) {
            printf("READ: data blocks: %d\n", blocks[i]);
        }
        printf("READ: number of blocks: %d\n", num_blocks);*/
    }


    //read the file block by block
    free(INode);

    char* open_file = calloc(BLOCK_SIZE * num_blocks, 1);
    char* open_block;
    for(i = 0; i < num_blocks; i++) {
        open_block = malloc(BLOCK_SIZE);
        readBlock(disk, blocks[i], open_block);
        strncat(open_file, open_block, BLOCK_SIZE);
        free(open_block);
    }
    
    return open_file;    
}

void createFile(FILE* disk, char* type, char* file, char* name, char* path) {
    
    if(strcmp(name, "root") == 0) {
        printf("CREATE: Cannot make file named 'root'\n");
        return;
    }
    if(strchr(name, '/') != NULL) {
        printf("CREATE: Cannot make file with '/' in its name\n");
        return;
    }
    //find free block
    //find number of blocks needed to store file rounding up
    int blocks_req = (strlen(file) + BLOCK_SIZE)/ BLOCK_SIZE;

    if(blocks_req > 10) {
        printf("CREATE: Cannot allocate a file larger than 10 blocks because that would be to reasonable.\n");
        return;
    }
    int* block_addr = calloc(blocks_req, 1);
    int i;
    int j;

    //get the first 1 bit in superblock
    for(j = 0; j < blocks_req; j++) {
        block_addr[j] = findFreeBlock(disk);
        if(block_addr[j] == 0) {
            printf("CREATE: FIND BLOCK FAILED, DISK MIGHT BE FULL\n");
            return;
        }
    }

    if(debug) {
        for(i = 0; i < blocks_req; i++) {
            printf("CREATE: address: %d\n", block_addr[i]);
        }        
    }

    //create INODE
    int INode_Addr = findFreeBlock(disk);
    int INode_Num = findFreeInode(disk, INode_Addr);
    printf("CREATE: INODE NUM: %d\n", INode_Num);
    printf("CREATE: INODE ADDR: %d\n", INode_Addr);
    char* INode;
    if(INode_Num != -1) {
        INode = createINode(disk, type, block_addr, INode_Num, 0);
    }
    else {
        printf("CREATE: Failed to Create File, INode Map full");
        return;
    }    

    if(debug == 1) {
        printf("CREATE: INode: %s\n", INode);
    }
    
    if(INode_Addr != 0) {
        writeBlock(disk, INode_Addr, INode);
    }
    else {
        printf("CREATE: Failed to create file, Disk Full");
    }

    //read blocks 2-9 and find location to write mapping
    //INodes take 2 bytes
    //int INode_block = (NUM_INODES / (BLOCK_SIZE / 2)) + 2;

    //write filename and inode number to appropriate directory
    INode_Num += '0';
    char* dir = malloc(BLOCK_SIZE);
    int dir_addr = 10;
    if(strcmp(path, "root") != 0) {
        dir_addr = findDirectoryAddr(disk, path);
        if(dir_addr == -1) {
            printf("CREATE: failed %s does not exist\n", path);
            return;
        }
        readBlock(disk, dir_addr, dir);
    }
    else {        
        readBlock(disk, dir_addr, dir);
    }
    for(i = 0; i < BLOCK_SIZE; i+=32) {
        //In DIRs INODE #s are stored in hex
        if(dir[i] == 0x00) {
            dir[i] = INode_Num;
            for(j = 1; j < strlen(name) + 1; j++) {
                dir[j+i] = name[j-1];
            }
            break;
        }
        else if (i == BLOCK_SIZE-32) {
            printf("CREATE: Directory Full, could not create %s\n", name);
            return;
        }
    }
    if(debug == 1) {
        /*printf("CREATE: dir: %s\n", dir);
        for(i = 0; i < BLOCK_SIZE; i++) {
            printf("CREATE: %c %d\n", dir[i], i);
        }*/
    }
    writeBlock(disk, dir_addr, dir);
    free(dir);

    //write file data
    if(type == "0000") {
        char* temp;
        for(i = 0; i < blocks_req; i++) {
            temp = malloc(BLOCK_SIZE);
            for(j = 0; j < BLOCK_SIZE; j++) {
                if(j >= strlen(file) - (i * BLOCK_SIZE)) {
                    break;
                }
                temp[j] = file[j + (i * BLOCK_SIZE)];
            }
            writeBlock(disk, block_addr[i], temp);
            free(temp);
        }
    }
    else {
        char* temp = calloc(BLOCK_SIZE, 1);
        writeBlock(disk, block_addr[0], temp);
    }
    
}

void writeToFile(FILE* disk, char* data, char* file_name, char* path) {
    //need to update for subdirectories
    char* dir = malloc(BLOCK_SIZE);
    int i;
    int j;
    int INode_Num;
    if(strcmp(path, "root") != 0) {
        int addr = findDirectoryAddr(disk, path);
        if(addr == -1) {
            printf("WRITE: failed %s does not exist\n", path);
            return;
        }
        readBlock(disk, addr, dir);
    }
    else {        
        readBlock(disk, 10, dir);
    }


    //check all filenames in current directory and get INODE_NUM if it exists
    for(i = 0; i < BLOCK_SIZE; i+=32) {
        j = 0;
        char* temp = malloc(strlen(file_name));
        while(j < strlen(file_name)) {
            temp[j] = dir[i + j + 1];
            j++;
        }
        if(strncmp(temp, file_name, strlen(file_name)) == 0) {
            INode_Num = dir[i] - '0';
            free(temp);
            break;
        }
        free(temp);

        if(i == BLOCK_SIZE-32) {
            printf("READ: file: %s does not exist in current directory\n", file_name);
            free(dir);
            return;
        }
    }
    //get Inode address from inode number
    free(dir);
    char INode_Addr[3];    
    findINodeAddr(disk, INode_Addr, INode_Num);
    int addr = (int)strtol(INode_Addr, NULL, 16);    //hex to int

    if(debug == 1) {
        printf("READ: INODE NUM: %d\n", INode_Num);      
        printf("READ: INODE ADDR: %s\n", INode_Addr);
    }

    //get inode from inode address
    char* INode = malloc(BLOCK_SIZE);
    readBlock(disk, addr, INode);
    int blocks[10] = { 0 };
    int num_blocks = 0;

    //get number of blocks and their addresses from INode bytes 8 - 27
    //TODO handle indirects
    for(i = 8; i < 27; i += 2) {
        if(INode[i] == '0' && INode[i+1] == '0') {
            break;
        }
        else {
            char temp[3] = { INode[i], INode[i+1], '\0' };
            blocks[(i-8)/2] = (int)strtol(temp, NULL, 16);
            num_blocks++;
        }
    }
    
    if(debug == 1) {
        /*for(i = 0; i < 10; i++) {
            printf("READ: data blocks: %d\n", blocks[i]);
        }
        printf("READ: number of blocks: %d\n", num_blocks);*/
    }

    //write data to existing blocks as well as creating new blocks
    int blocks_req = (strlen(data) + BLOCK_SIZE - 1)/ BLOCK_SIZE;
    printf("WRITE: req: %d num: %d\n", blocks_req, num_blocks);
    char* temp;

    if(blocks_req <= num_blocks) {
        for(i = 0 ; i < num_blocks; i++) {
            temp = calloc(BLOCK_SIZE, 1);
            for(j = 0; j < BLOCK_SIZE; j++) {
                //TODO delete unnessary blocks - currently inputs some garbage values if smaller
                if(j >= (strlen(data) - (i * BLOCK_SIZE))) {
                    break;
                }
                temp[j] = data[j + (i * BLOCK_SIZE)];
            }
            //printf("WRITE: block: %d data: %s\n", blocks[i], temp);
            writeBlock(disk, blocks[i], temp);
            free(temp);
        }
    }
    else { //need to create more blocks TODO handle indirects
        //allocate new blocks
        int new_blocks = blocks_req - num_blocks;
        for(i = 0; i < new_blocks; i++) { //currently assumes less than 10
            blocks[i + num_blocks] = findFreeBlock(disk);
        }
        //write data to all blocks (new and old)
        for(i = 0 ; i < blocks_req; i++) {
            temp = calloc(BLOCK_SIZE, 1);
            for(j = 0; j < BLOCK_SIZE; j++) {
                if(j >= (strlen(data) - (i * BLOCK_SIZE))) {
                    break;
                }
                temp[j] = data[j + (i * BLOCK_SIZE)];
            }
            //printf("WRITE: block: %d data: %s\n", blocks[i], temp);
            writeBlock(disk, blocks[i], temp);
            free(temp);
        }

        //add new addresses to INODE
        for(i = 8 + (new_blocks * 2); i < 27; i += 2) {
            int z = ((i - (8 + (new_blocks * 2)))/2) + new_blocks; //Gross but works
            if(blocks[z] == 0) {
                INode[i] = '0';
                INode[i+1] = '0';
            }
            else {
                INode[i] = TO_HEX(((blocks[z] & 0xF0) >> 4));
                INode[i+1] = TO_HEX((blocks[z] & 0x0F));
            }
        }
        writeBlock(disk, addr, INode);
        
    }
    if(debug == 1) {
        printf("WRITE: INode: %s\n", INode);
    }    
    free(INode);
}

void deleteFile(FILE* disk, char* file_name, char* path) {
    char* dir = malloc(BLOCK_SIZE);
    int i;
    int j;
    int INode_Num;
    if(strcmp(path, "root") != 0) {
        int addr = findDirectoryAddr(disk, path);
        if(addr == -1) {
            printf("DELETE: failed %s does not exist\n", path);
            return;
        }
        readBlock(disk, addr, dir);
    }
    else {        
        readBlock(disk, 10, dir);
    }

    //check all filenames in current directory and get INODE_NUM if it exists
    for(i = 0; i < BLOCK_SIZE; i+=32) {
        j = 0;
        char* temp = malloc(strlen(file_name));
        while(j < strlen(file_name)) {
            temp[j] = dir[i + j + 1];
            j++;
        }
        if(strncmp(temp, file_name, strlen(file_name)) == 0) {
            INode_Num = dir[i] - '0';
            free(temp);
            break;
        }
        free(temp);

        if(i == BLOCK_SIZE-32) {
            printf("READ: file: %s does not exist in current directory\n", file_name);
            free(dir);
            return;
        }
    }

    //get Inode address from inode number
    free(dir);
    char INode_Addr[3];    
    findINodeAddr(disk, INode_Addr, INode_Num);
    int addr = (int)strtol(INode_Addr, NULL, 16);    //hex to int

    if(debug == 1) {
        printf("DELETE: INODE NUM: %d\n", INode_Num);      
        printf("DELETE: INODE ADDR: %s\n", INode_Addr);
    }

    //get inode from inode address
    char* INode = malloc(BLOCK_SIZE);
    readBlock(disk, addr, INode);

    if(debug == 1) {
        printf("DELETE: INode: %s\n", INode);
    }
    

    //check if directory - must be empty
    if(INode[4] == '1') {
        char temp_path[strlen(path) + strlen(file_name) + 1];
        strncpy(temp_path, path, strlen(path)+1);
        strncat(temp_path, "/", 1);
        strncat(temp_path, file_name, strlen(file_name));
        if(debug == 1) {
            printf("DELETE: temp_path: %s\n", temp_path);
        }
        int dir_addr = findDirectoryAddr(disk, temp_path);
        dir = malloc(BLOCK_SIZE);
        readBlock(disk, dir_addr, dir);
        for(i = 0; i < BLOCK_SIZE; i += 32) {
            if(dir[i] != 0) {
                printf("DELETE: Failed, cannot delete a non-empty directory\n");
                free(dir);
                free(INode);
                return;
            }
        }
    }

    int blocks[10] = { 0 };
    int num_blocks = 0;

    //get number of blocks and their addresses from INode bytes 8 - 27
    //TODO handle indirects
    for(i = 8; i < 27; i += 2) {
        if(INode[i] == '0' && INode[i+1] == '0') {
            break;
        }
        else {
            char temp[3] = { INode[i], INode[i+1], '\0' };
            blocks[(i-8)/2] = (int)strtol(temp, NULL, 16);
            num_blocks++;
        }
    }
}

int main (int argc, char* argv[]) {
    FILE* disk = fopen("../disk/vdisk", "r+b");

    //if disk needs to be created
    if(argc > 1) {
        if(strcmp(argv[1], "init") == 0) {
            createDisk(disk);
            //printf("size of root_init: %lu\n", strlen(root_init));
            //createFile(disk, "1111", '\0', "root");
            //free(root_init);
        }
    }

    char* file_con = "Gt85LeGhaEs7iGp7khXXrMMk08KFNBJhVroxE42rv6uEHSUBEjy9OHLhdj0lMFyHywAhJgz0XLky9PDUDFrPin7i6W5wAlQ6V53fUExBNkK9VNzFhgwG3ajig9ph9FhhCIeeA5UmsFD6Qnt8zREmAWFGO7qCJUwcCfrO8plpapGTC7apP2lGiQkj1sypu6WRcJFNidIcNTvDu7nJQJngUr9j7mye81iCQP1OnIunkV7Ho8rLyC5ZOPx3y9oY0XYtzIQkfRdFRwIHwVkXBSAq0dw8JUB8if6OAyC5bIEZiqnku7MwMuEo5a2k9Im3STJRzNwwsPjiiT9470DWqRVvMVBYnQDnciD0iIcdoPJhr0phzAhSCfbrEKYbeAD64UTBeXCRowwcrWKaO95GFI0r0pyCo6arOI3BgXVOH7mVYvMRQ7oF8R10Vauv7iwuCxJ4fE9gXwIG80uLAw35NIUz1BC39yobFehFfUMc05cBirZjWN6HcOh7jzOuPUwfDs4GgGR5IOWmCmE9C1Cr7ryCy6D6coazAE1Jr8IzmKkBIG7yyl5n42baolbQhO0bg7IZ9tg74h2ZrSAAmXhYNSM8NCSiRoD398r3A74ubVKFmdy2u7S3SrVWPpw0ZO3C8w5kGXfrAcGRXK4rYD1YwRkgDRHhI0OxQm33NxSzVqWQrUKQQnNE9x9Sxp6bLtzozy6Jl2Bjp4601mrDr9YF4IvtsfmGjYSekLqWDKzoR9QdPgUyV4CA4UoiS1AnZpDGDGQmwVPPl9O3vNsJ4zl9xs5txQ3xWuc2ZOtJJCymHvBeAiz7uy5WgjND1EZ1HbCvpScilubojqX89EqFllK76bnDK1rearuCZRWuEhdEkyzqEg2Ri3Inf5xT4V1bhZ6iKsPOT3K5RpKVGmpf43VFqZgmDCCMShPstalFVkYwdJMbaxctELcuX7IuzoGzN3d9xVeJFLiMTdHjeggz185NTaIJGsFqQON5CWKMdIdmkuymxJv0ICMCmi6LEGcw3hgBkbNCrQBGHeICHdQoRvjuX5HvYTkjIDva8lZz1SZOANdUF6kkGZ07HXWm9114DHelZSf2YyDev95YFZPC1xO1xWKgX0JPdIV5xjYGIj1BzdBfcSgU00I8SyvzfJRHPdRDv6dCu1Gr7CW0ZJtpjDNkwpHbRao0laJrOiNhURACtZy8Us3cUxthw7J5g9YX3DTpCrpYwjaQOOLGWfZaTYwA0ixPCypAzjlqMqF6FOBr7vHnbifYijC76eZoezSsf28zejLahY29ry3mTit3CM3FPD4hVVqvaP6hU5JfDALzh1ATGfvLutBAD0VJp7UrTtSTazeALfnN7zOwN5M475sNYhcbhUsIi5e7bPXQiLmJAj4Gt0dFCJ18kNvfNADRZaOyNg3jx4Wqks6vQcL9qSH6a9tADP9KXQLqZagXkivtus4UGzvHyTHp6yPZ1kHnVzRXW3XvNMXf7wSdrCB9jYWiab18i9BXbduS";
    printf("%lu\n", strlen(file_con));
    char path[] = "root";
    createFile(disk, "0000", file_con, "foobar", path);
    createFile(disk, "1111", "", "documents", path);
    //char path2[] = "root/documents";
    //createFile(disk, "0000", file_con, "foobar2", path2);

   // char* newfoobar = "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";
    //writeToFile(disk, newfoobar, "foobar2", path2);

    //char* foobar2 = readFile(disk, "foobar2", path2);
    //if(foobar2 != '\0') {
        //printf("Read File:\n%s\n", foobar2);
        //free(foobar2);
    //}
    deleteFile(disk, "documents", path);
    fclose(disk);
}

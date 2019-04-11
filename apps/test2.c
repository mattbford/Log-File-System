#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../io/file.h"

int main (int argc, char* argv[]) {
    FILE* disk = fopen("../disk/vdisk", "r+b");

    char* file_con = "Gt85LeGhaEs7iGp7khXXrMMk08KFNBJhVroxE42rv6uEHSUBEjy9OHLhdj0lMFyHywAhJgz0XLky9PDUDFrPin7i6W5wAlQ6V53fUExBNkK9VNzFhgwG3ajig9ph9FhhCIeeA5UmsFD6Qnt8zREmAWFGO7qCJUwcCfrO8plpapGTC7apP2lGiQkj1sypu6WRcJFNidIcNTvDu7nJQJngUr9j7mye81iCQP1OnIunkV7Ho8rLyC5ZOPx3y9oY0XYtzIQkfRdFRwIHwVkXBSAq0dw8JUB8if6OAyC5bIEZiqnku7MwMuEo5a2k9Im3STJRzNwwsPjiiT9470DWqRVvMVBYnQDnciD0iIcdoPJhr0phzAhSCfbrEKYbeAD64UTBeXCRowwcrWKaO95GFI0r0pyCo6arOI3BgXVOH7mVYvMRQ7oF8R10Vauv7iwuCxJ4fE9gXwIG80uLAw35NIUz1BC39yobFehFfUMc05cBirZjWN6HcOh7jzOuPUwfDs4GgGR5IOWmCmE9C1Cr7ryCy6D6coazAE1Jr8IzmKkBIG7yyl5n42baolbQhO0bg7IZ9tg74h2ZrSAAmXhYNSM8NCSiRoD398r3A74ubVKFmdy2u7S3SrVWPpw0ZO3C8w5kGXfrAcGRXK4rYD1YwRkgDRHhI0OxQm33NxSzVqWQrUKQQnNE9x9Sxp6bLtzozy6Jl2Bjp4601mrDr9YF4IvtsfmGjYSekLqWDKzoR9QdPgUyV4CA4UoiS1AnZpDGDGQmwVPPl9O3vNsJ4zl9xs5txQ3xWuc2ZOtJJCymHvBeAiz7uy5WgjND1EZ1HbCvpScilubojqX89EqFllK76bnDK1rearuCZRWuEhdEkyzqEg2Ri3Inf5xT4V1bhZ6iKsPOT3K5RpKVGmpf43VFqZgmDCCMShPstalFVkYwdJMbaxctELcuX7IuzoGzN3d9xVeJFLiMTdHjeggz185NTaIJGsFqQON5CWKMdIdmkuymxJv0ICMCmi6LEGcw3hgBkbNCrQBGHeICHdQoRvjuX5HvYTkjIDva8lZz1SZOANdUF6kkGZ07HXWm9114DHelZSf2YyDev95YFZPC1xO1xWKgX0JPdIV5xjYGIj1BzdBfcSgU00I8SyvzfJRHPdRDv6dCu1Gr7CW0ZJtpjDNkwpHbRao0laJrOiNhURACtZy8Us3cUxthw7J5g9YX3DTpCrpYwjaQOOLGWfZaTYwA0ixPCypAzjlqMqF6FOBr7vHnbifYijC76eZoezSsf28zejLahY29ry3mTit3CM3FPD4hVVqvaP6hU5JfDALzh1ATGfvLutBAD0VJp7UrTtSTazeALfnN7zOwN5M475sNYhcbhUsIi5e7bPXQiLmJAj4Gt0dFCJ18kNvfNADRZaOyNg3jx4Wqks6vQcL9qSH6a9tADP9KXQLqZagXkivtus4UGzvHyTHp6yPZ1kHnVzRXW3XvNMXf7wSdrCB9jYWiab18i9BXbduS";
    //printf("%lu\n", strlen(file_con));
    char path[] = "root";
    int temp;

    //createDisk(disk);

    printf("Commencing Test 2\n");

    //step 1
    printf("Step 1: Creating file 'foobar' in %s\n", path);
    temp = createFile(disk, "0000", file_con, "foobar", path);
    if(temp != -1) {
        printf("Step 1: successfully created 'foobar' in %s\n", path);
        char* hold = readFile(disk, "foobar", path);
        printf("hold %s\n", hold);
        printf("file_con %s\n", file_con);
        if(strcmp(hold, file_con) == 0) {
            printf("Step 1: read returns expected string\n");
        }
        else {
            printf("Step 1: read returns unexpected string\n");
            //exit(-1);
        }
    }
    else {
        printf("Step 1: failed to create 'foobar' in %s. test2 failed: terminate..\n", path);
        exit(-1);
    }

    //step 2
    printf("Step 2: Creating directory 'documents' in %s\n", path);
    temp = createFile(disk, "1111", "", "documents", path);
    if(temp != -1) {
        printf("Step 2: successfully created 'documents' in %s\n", path);
        char* hold = readFile(disk, "documents", path);
        if(strcmp(hold, "") == 0) {
            printf("Step 2: read returns expected string\n");
        }
        else {
            printf("Step 2: read returns unexpected string\n");
            exit(-1);
        }
    }
    else {
        printf("Step 2: failed to create 'documents' in %s. test2 failed: terminate..\n", path);
        exit(-1);
    }

    char path2[] = "root/documents";
    
    //step 3
    printf("Step 3: Creating file 'foobar2'in %s\n", path2);
    temp = createFile(disk, "0000", file_con, "foobar2", path2);
    if(temp != -1){
        printf("Step 3: successfully created 'foobar2' in %s\n", path2);
        char* hold = readFile(disk, "foobar2", path2);
        if(strcmp(hold, file_con) == 0) {
            printf("Step 3: read returns expected string\n");
        }
        else {
            printf("Step 3: read returns unexpected string\n");
            //exit(-1);
        }
    }
    else{
        printf("Step 3: failed to create 'foobar2' in %s. test2 failed: terminate..\n", path);
        exit(-1);
    }

    //step 4
    printf("Step 4: writing to file 'foobar2'in %s\n", path2);
    char* newfoobar = "Gt85LeGhaEs7iGp7khXXrMMk08KFNBJhVroxE42rv6uEHSUBEjy9OHLhdj0lMFyHywAhJgz0XLky9PDUDFrPin7i6W5wAlQ6V53fUExBNkK9VNzFhgwG3ajig9ph9FhhCIeeA5UmsFD6Qnt8zREmAWFGO7qCJUwcCfrO8plpapGTC7apP2lGiQkj1sypu6WRcJFNidIcNTvDu7nJQJngUr9j7mye81iCQP1OnIunkV7Ho8rLyC5ZOPx3y9oY0XYtzIQkfRdFRwIHwVkXBSAq0dw8JUB8if6OAyC5bIEZiqnku7MwMuEo5a2k9Im3STJRzNwwsPjiiT9470DWqRVvMVBYnQDnciD0iIcdoPJhr0phzAhSCfbrEKYbeAD64UTBeXCRowwcrWKaO95GFI0r0pyCo6arOI3BgXVOH7mVYvMRQ7oF8R10Vauv7iwuCxJ4fE9gXwIG80uLAw35NIUz1BC39yobFehFfUMc05cBirZjWN6HcOh7jzOuPUwfDs4GgGR5IOWmCmE9C1Cr7ryCy6D6coazAE1Jr8IzmKkBIG7yyl5n42baolbQhO0bg7IZ9tg74h2ZrSAAmXhYNSM8NCSiRoD398r3A74ubVKFmdy2u7S3SrVWPpw0ZO3C8w5kGXfrAcGRXK4rYD1YwRkgDRHhI0OxQm33NxSzVqWQrUKQQnNE9x9Sxp6bLtzozy6Jl2Bjp4601mrDr9YF4IvtsfmGjYSekLqWDKzoR9QdPgUyV4CA4UoiS1AnZpDGDGQmwVPPl9O3vNsJ4zl9xs5txQ3xWuc2ZOtJJCymHvBeAiz7uy5WgjND1EZ1HbCvpScilubojqX89EqFllK76bnDK1rearuCZRWuEhdEkyzqEg2Ri3Inf5xT4V1bhZ6iKsPOT3K5RpKVGmpf43VFqZgmDCCMShPstalFVkYwdJMbaxctELcuX7IuzoGzN3d9xVeJFLiMTdHjeggz185NTaIJGsFqQON5CWKMdIdmkuymxJv0ICMCmi6LEGcw3hgBkbNCrQBGHeICHdQoRvjuX5HvYTkjIDva8lZz1SZOANdUF6kkGZ07HXWm9114DHelZSf2YyDev95YFZPC1xO1xWKgX0JPdIV5xjYGIj1BzdBfcSgU00I8SyvzfJRHPdRDv6dCu1Gr7CW0ZJtpjDNkwpHbRao0laJrOiNhURACtZy8Us3cUxthw7J5g9YX3DTpCrpYwjaQOOLGWfZaTYwA0ixPCypAzjlqMqF6FOBr7vHnbifYijC76eZoezSsf28zejLahY29ry3mTit3CM3FPD4hVVqvaP6hU5JfDALzh1ATGfvLutBAD0VJp7UrTtSTazeALfnN7zOwN5M475sNYhcbhUsIi5e7bPXQiLmJAj4Gt0dFCJ18kNvfNADRZaOyNg3jx4Wqks6vQcL9qSH6a9tADP9KXQLqZagXkivtus4UGzvHyTHp6yPZ1kHnVzRXW3XvNMXf7wSdrCB9jYWiab18i9BXbduSGt85LeGhaEs7iGp7khXXrMMk08KFNBJhVroxE42rv6uEHSUBEjy9OHLhdj0lMFyHywAhJgz0XLky9PDUDFrPin7i6W5wAlQ6V53fUExBNkK9VNzFhgwG3ajig9ph9FhhCIeeA5UmsFD6Qnt8zREmAWFGO7qCJUwcCfrO8plpapGTC7apP2lGiQkj1sypu6WRcJFNidIcNTvDu7nJQJngUr9j7mye81iCQP1OnIunkV7Ho8rLyC5ZOPx3y9oY0XYtzIQkfRdFRwIHwVkXBSAq0dw8JUB8if6OAyC5bIEZiqnku7MwMuEo5a2k9Im3STJRzNwwsPjiiT9470DWqRVvMVBYnQDnciD0iIcdoPJhr0phzAhSCfbrEKYbeAD64UTBeXCRowwcrWKaO95GFI0r0pyCo6arOI3BgXVOH7mVYvMRQ7oF8R10Vauv7iwuCxJ4fE9gXwIG80uLAw35NIUz1BC39yobFehFfUMc05cBirZjWN6HcOh7jzOuPUwfDs4GgGR5IOWmCmE9C1Cr7ryCy6D6coazAE1Jr8IzmKkBIG7yyl5n42baolbQhO0bg7IZ9tg74h2ZrSAAmXhYNSM8NCSiRoD398r3A74ubVKFmdy2u7S3SrVWPpw0ZO3C8w5kGXfrAcGRXK4rYD1YwRkgDRHhI0OxQm33NxSzVqWQrUKQQnNE9x9Sxp6bLtzozy6Jl2Bjp4601mrDr9YF4IvtsfmGjYSekLqWDKzoR9QdPgUyV4CA4UoiS1AnZpDGDGQmwVPPl9O3vNsJ4zl9xs5txQ3xWuc2ZOtJJCymHvBeAiz7uy5WgjND1EZ1HbCvpScilubojqX89EqFllK76bnDK1rearuCZRWuEhdEkyzqEg2Ri3Inf5xT4V1bhZ6iKsPOT3K5RpKVGmpf43VFqZgmDCCMShPstalFVkYwdJMbaxctELcuX7IuzoGzN3d9xVeJFLiMTdHjeggz185NTaIJGsFqQON5CWKMdIdmkuymxJv0ICMCmi6LEGcw3hgBkbNCrQBGHeICHdQoRvjuX5HvYTkjIDva8lZz1SZOANdUF6kkGZ07HXWm9114DHelZSf2YyDev95YFZPC1xO1xWKgX0JPdIV5xjYGIj1BzdBfcSgU00I8SyvzfJRHPdRDv6dCu1Gr7CW0ZJtpjDNkwpHbRao0laJrOiNhURACtZy8Us3cUxthw7J5g9YX3DTpCrpYwjaQOOLGWfZaTYwA0ixPCypAzjlqMqF6FOBr7vHnbifYijC76eZoezSsf28zejLahY29ry3mTit3CM3FPD4hVVqvaP6hU5JfDALzh1ATGfvLutBAD0VJp7UrTtSTazeALfnN7zOwN5M475sNYhcbhUsIi5e7bPXQi";
    temp = writeToFile(disk, newfoobar, "foobar2", path2);
    if(temp != -1) {
        printf("Step 4: successfully wrote to 'foobar2' in %s\n", path2);
        char* hold = readFile(disk, "foobar2", path2);
        if(strcmp(hold, newfoobar) == 0) {
            printf("Step 4: read returns expected string\n");
        }
        else {
            printf("Step 4: read returns unexpected string\n");
            //exit(-1);
        }
    }
    else {        
        printf("Step 4: failed to write to'foobar2' in %s. test2 failed: terminate..\n", path2);
        exit(-1);
    }
    //step 5
    printf("Step 5: Deleting directory 'foobar2' in %s\n", path2);
    temp = deleteFile(disk, "foobar2", path2);
    if(temp != -1) {
        printf("Step 6: successfully deleted 'foobar2' from %s", path2);
    }
    else{
        printf("Step 6: failed to delete 'foobar2' from %s. test2 failed: terminate..\n", path2);
        exit(-1);
    }

    //step 6
    printf("Step 6: Deleting directory 'documents' in %s\n", path);
    temp = deleteFile(disk, "documents", path);
    if(temp != -1) {
        printf("Step 6: successfully deleted 'documents' from %s", path);
    }
    else{
        printf("Step 6: failed to delete 'documents' from %s. test2 failed: terminate..\n", path);
        exit(-1);
    }
    fclose(disk);

    printf("Test 2 successful\n\n");
}
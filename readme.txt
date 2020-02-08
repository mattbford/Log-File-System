The "Little" Log File System
-by Matthew Belford

Currently my system has the following functionality in place:
    -Simulation of a file system
    -create files
    -read files
    -write files
    -create, read, write directories (including subdirectories)
    -delete files and (empty) subdirectories
    -robustness (kind of)

Simulation of a file system:
    I created 4096 blocks of size 512 bytes each.
    The first 10 blocks are reserved  with block 0 being the superblock, block 1 being the freeblock vector and I reappropriated 
    blocks 2-9 as the Inode mapping. The freeblock vector uses a single bit to represent which blocks are free and which are taken.
    This is calculated using a lot of logic math that I don't really want to talk about. It is highlighted in the findFreeBlock         
    functionality if you are curious. The Inode mapping breaks each two bytes down into a big endian hex number which is equal to the 
    block number (in integer form) in which that corresponding INode is stored. findFreeINode() returns the inode number of the first
    section of the Inode mapping that is empty starting from block 2. findINodeAddr takes an INode number and finds its corresponding
    address in the INode mapping. The one feature that did not make it into this filesystem is the concept of single and doubly
    indirects, so file sizes are capped at 10 blocks. I didn't have time to finish it and I thought it would be far more interesting to 
    implement the deleteFile function instead. Furthermore, I did not implement caching as it would require heavy refactoring and the 
    system is a lot more robust without it as data is written immediately and will not be lost in event of a crash.

Create Files:
    An absolute behemoth of a system that I totally underestimated.
    The shear number of things that the createFile() function has to build and keep track of is massive. I got my first taste of how 
    big this system was going to be when I began to construct the INodes. Overall I did not have too many scalability issues with the 
    createFile() function and I would argue it is probably some of the best code that is written in this system. However, I did find 
    myself coming back to it repeatedly to iron out bugs and add new features such as subdirectories.

Read and Write Files:
    The functions readFile() and writeFile() suffered from major scalability problems from the beginning. As new features and 
    specifications were added, the functions needed to be altered very carefully. The amount of times I broke these functions is 
    uncountable. Everytime I thought I had it, I realized that I had one more feature to add that would completely change the format of 
    the function. Overall both functions are jumbled messes that share a lot of code. There are large chunks of code that could be 
    exported into their own functions but I was unable due to time constraints. A list of features that broke these functions:
        INodes
        Indirects
        subdirectories
        test cases
        deletes

Subdirectories:
    This was the quintessential "stay up till 3am coding" experience. I didn't know what broken meant before I attempted to add 
    directories. I built the findDirectoryAddr() function to deal with this problem. It tokenizes the path and searches through the 
    filesystem from each step eventually returning the integer address of the head of the path. I realized about half-way through 
    implementing directories that a more elegant solution would be to include the path in the file_name parameter, but I was too far 
    gone and the system works nonetheless. Needless to say, I don't really want to talk about it.

Deletion:
    Surprisingly easy. It was nearly a copy-paste of different sections of features I had already implemented. The following was my 
    process:
        -make sure file exists
        -get Inode
        -if directory make sure its empty
        -erase directory info from parent directory
        -overwrite all block data to null
        -write block as available in freeblock vector
        -write Inode number as available in INode mapping
        -overwrite all inode data to null
    I didn't have any problems with this feature as it magically worked first try.

Robustness(kinda):
    Due to the lack of a cache, the filesystem is partially robust. Data will not be lost in the event of a crash. The one problem is 
    that if the filesystem crashes mid createFile() then there could be some untraceable resource usage eg. INodes nums that are mapped 
    to empty addresses, etc. If I learned anything from Database Systems is that we have three aspects to a system: Speed, Durability, 
    and Cost. We can only ever satisfy two of those aspects. I chose durability and cost(my time and sanity) and as a result the system 
    definitely isn't very efficient.

The tests currently don't work. CreateFile is not adding the file data to the disk for some reason. However if the test code is copied into file.c's main it works correctly. Bascially test1 test create and read including subdirectories as well as the deletion of an empty directory. Test2 tests create, read, write, and deletion. I have no clue why this is happening and due to time constraints I am unable to investigate further.

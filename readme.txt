The Little "I'm not really sure what to call this anymore" Log(?) File System
-by Matthew Belford

Warning to the reader: If you are looking at this code and thinking "what the heck is going on" just remember Yvonne made me do it.
In all seriousness though, thank you for an excellent semester! This project, though hard, was very rewarding and interesting.

Currently my system has the following functionality in place:
    -Simulation of a file system
    -create files
    -read files
    -write files
    -create, read, write directories (including subdirectories)
    -delete files and (empty) subdirectories
    -robustness (kind of)

Simulation of a file system:
    I tried my best to follow the specifications of the LLFS document provided. I create 4096 blocks of size 512 bytes each.
    The first 10 blocks are reserved  with block 0 being the superblock, block 1 being the freeblock vector and I reappropriated 
    blocks 2-9 as the Inode mapping. The freeblock vector uses a single bit to represent which blocks are free and which are taken.
    This is calculated using a lot of logic math that I don't really want to talk about. It is highlighted in the findFreeBlock functionality
    if you are curious. The Inode mapping breaks each two bytes down into a big endian hex number which is equal to the block number
    (in integer form) in which that corresponding INode is stored. findFreeINode() returns the inode number of the first section of the Inode
    mapping that is empty starting from block 2. findINodeAddr takes an INode number and finds its corresponding address in the INode mapping.
    The one feature that did not make it into this filesystem is the concept of single and doubly indirects, so file sizes are capped at 10 blocks.
    I really wish I had time to implement it, but I thought it would be far more interesting to implement the deleteFile function instead. Furthermore,
    I did not implement the caching approach, partly because I was already in too deep and partly because I figured that the filesystem would be
    far more reliable if all data was written immediately(not realizing that robustness was goal 3).

Create Files:
    An absolute behemoth of a system that I totally underestimated. I thought I would be able to write this and be done with it but I was very wrong.
    The shear number of things that the createFile() function has to build and keep track of is massive. I got my first taste of how big this
    system was going to be when I began to construct the INodes. Overall I did not have too many scalability issues with the createFile() function
    and I would argue it is probably some of the best code that I wrote in this assignment. However, I did find myself coming back to it
    repeatedly to iron out bugs and add new features such as subdirectories.

Read and Write Files:
    The functions readFile() and writeFile() suffered from major scalability problems from the beginning. As new features and specifications were
    added, the functions needed to be altered very carefully. This was the most frustrating part of the assignment. The amount of times I broke
    these functions is uncountable. Everytime I thought I had it, I realized that I had one more feature to add that would completely change 
    the format of the function. Overall both functions are jumbled messes that share a lot of code. There are large chunks of code that could
    be exported into their own functions but due to time constraints I chose to not bother with this approach. A list of features that broke
    these functions:
        INodes
        Indirects
        subdirectories
        test cases
        deletes

Subdirectories:
    This was the quintessential "stay up till 3am coding" experience. I didn't know what broken meant before I attempted to add directories.
    I built the findDirectoryAddr() function to deal with this problem. It tokenizes the path and searches through the filesystem from each step
    eventually returning the integer address of the head of the path. I realized about half-way through implementing directories that a more
    elegant solution would be to include the path in the file_name parameter, but I was too far gone and the system works nonetheless.
    Needless to say, I don't really want to talk about it.

Deletion:
    Surprisingly easy. It was nearly a copy-paste of different sections of features I had already implemented. The following was my process:
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
    Due to the lack of a cache, the filesystem is partially robust. Data will not be lost in the event of a crash. The one problem is that
    if the filesystem crashes mid createFile() then there could be some untraceable resource usage eg. INodes nums that are mapped to empty
    addresses, etc. If I learned anything from Database Systems is that we have three aspects to a system: Speed, Durability, and Cost. 
    We can only ever satisfy two of those aspects. I chose durability and cost(my time and sanity) and as a result the system definitely isn't
    very efficient.


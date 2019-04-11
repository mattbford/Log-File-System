DISK = ../disk
APPS = ../apps

test1: $(APPS)/test1.o file.o $(DISK)/disk.o
	gcc -g -std=gnu11 -o $(APPS)/test1 $(APPS)/test1.o file.o $(DISK)/disk.o
test2: $(APPS)/test2.o file.o $(DISK)/disk.o
	gcc -g -std=gnu11 -o $(APPS)/test2 $(APPS)/test2.o file.o $(DISK)/disk.o
disk.o: $(DISK)/disk.c
	gcc -g -std=gnu11 -c $(DISK)/disk.c
file.o: file.c
	gcc -g -std=gnu11 -c file.c
test1.o: $(APPS)/test1.c
	gcc -g -std=gnu11 -c $(APPS)/test1.c
test2.o: $(APPS)/test2.c
	gcc -g -std=gnu11 -c $(APPS)/test2.c
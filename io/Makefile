# filesystem make file
DISK = ../disk
APPS = ../apps
TARGETS1 = $(DISK)/disk file $(APPS)/test1
TARGETS2 = $(DISK)/disk file $(APPS)/test2
OBJS1 = file.o $(DISK)/disk.o $(APPS)/test1.o 
OBJS2 = file.o $(DISK)/disk.o $(APPS)/test2.o
CFLAGS += -g -std=gnu11
test1: $(TARGETS1)
$(TARGETS1): $(OBJS1)

test2: $(TARGETS2)
$(TARGETS2): $(OBJS2)

clean:
	rm -f $(TARGETS1) $(OBJS1) $(APPS)/test2.o $(APPS)/test2

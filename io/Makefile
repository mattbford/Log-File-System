# filesystem make file
DISK = ../disk
APPS = ../apps
TARGETS = $(DISK)/disk file $(APPS)/test1
OBJS = file.o $(DISK)/disk.o $(APPS)/test1.o
CFLAGS += -g -std=gnu11
all: $(TARGETS)
$(TARGETS): $(OBJS)
clean:
	rm -f $(TARGETS) $(OBJS)

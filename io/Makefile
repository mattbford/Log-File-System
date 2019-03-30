# filesystem make file
DISK = ../disk
TARGETS = $(DISK)/disk file
OBJS = file.o $(DISK)/disk.o
CFLAGS += -g -std=gnu11
all: $(TARGETS)
$(TARGETS): $(OBJS)
clean:
	rm -f $(TARGETS) $(OBJS)

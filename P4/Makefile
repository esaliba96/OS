CC = gcc
CFLAGS = -g

all: fs_Sim

fs_Sim: tinyFS.c
	$(CC) $(CFLAGS) -o fs_Sim tinyFS.c libDisk.c

clean:
			rm -f fs_Sim

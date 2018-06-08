LIBS =
OFILES = libTinyFS.o libDisk.o

#append a 32 to executable name if on a 32 bit machine
FILE =
ARCH = $(shell arch)
ifeq ("$(ARCH)", "i686")
   FILE = 32
endif

all:	$(FILE) fs_Sim$(FILE) cleano

32: 
	# to make life easier - automatically deletes .o's that were most likely created on a 64 bit machine
	# assumes you are doing most of your development on 64 bit machines
	rm -f *.o


fs_Sim$(FILE): tinyFsDemo.c libTinyFS.o libDisk.o
	$(CC) $(CFLAGS)-o tinyFsDemo$(FILE) tinyFsDemo.c $(OFILES) $(LIBS)

.c.o:
	gcc -c $(CFLAGS) $< -o $@ $(LIBS)

cleano:
	rm -f *.o

clean:
	rm -f tinyFsDemo *.o

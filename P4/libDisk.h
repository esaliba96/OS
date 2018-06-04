#ifndef _libDisk_H_
#define _libDisk_H_

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <sys/time.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <strings.h>


#define NBYTES_ERR -5
#define ERR_ACCESS_FILE -6
#define ERR_RESIZE -4
#define BLOCKSIZE 256

int openDisk(char *filename, int nBytes);
int readBlock(int disk, int bNum, void *block);
int writeBlock(int disk, int bNum, void *block);
void closeDisk(int disk);

#endif
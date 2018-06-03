#ifndef _TinyFS_H_
#define _TinyFS_

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <sys/time.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <strings.h>

#define BLOCKSIZE 256
#define DEFAULT_DISK_SIZE 10240 
#define DEFAULT_DISK_NAME “tinyFSDisk”   
typedef int fileDescriptor;

typedef struct block{
	uint8_t buffer[BLOCKSIZE];
}__attribute__((packed)) block;


int tfs_mkfs(char *filename, int nBytes);
block init_blocks(int type);
int tfs_mount(char *filename);
int tfs_unmount(void);
int tfs_openFile(char *name);
int tfs_closeFile(fileDescriptor FD);
int tfs_writeFile(fileDescriptor FD, char *buffer, int size);
int tfs_deleteFile(fileDescriptor FD);
int tfs_readByte(fileDescriptor FD, char *buffer);
int tfs_seek(fileDescriptor FD, int offset);

#endif
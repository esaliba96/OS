#ifndef _tinyFS_H_
#define _tinyFS_H

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
#include "libDisk.h"

#define BLOCKSIZE 256
#define DEFAULT_DISK_SIZE 10240 
#define DEFAULT_DISK_NAME “tinyFSDisk” 

int diskNO; 
typedef int fileDescriptor;

typedef struct block{
	uint8_t buffer[BLOCKSIZE];
}__attribute__((packed)) block;

typedef struct fileblock{
	uint8_t blocktype;
	uint8_t magic_number;
	uint8_t next_block;
	uint8_t buffer[BLOCKSIZE-3];
}__attribute__((packed)) fileblock;

typedef struct inodeblock{
	uint8_t blocktype;
	uint8_t magic_number;
	uint8_t next_inode;
	uint64_t filename;
	uint8_t file_pointer;
	//creation time
	//last access
	uint8_t buffer[BLOCKSIZE-12];
}__attribute__((packed)) inodeblock;

typedef struct superblock{
	uint8_t blocktype;
	uint8_t magic_number;
	uint8_t free_block_pointer;
	uint8_t free_block_count;
	uint8_t buffer[BLOCKSIZE-12];
}__attribute__((packed)) superblock;

typedef struct fdNode {
	int data;
	struct fdNode* next;
} fdNode;


int tfs_mkfs(char *filename, int nBytes);
block init_blocks(int type);
int verify_fs(void);
int tfs_mount(char *filename);
int tfs_unmount(void);
int tfs_openFile(char *name);
int tfs_closeFile(fileDescriptor FD);
int tfs_writeFile(fileDescriptor FD, char *buffer, int size);
int tfs_deleteFile(fileDescriptor FD);
int tfs_readByte(fileDescriptor FD, char *buffer);
int tfs_seek(fileDescriptor FD, int offset);

#endif
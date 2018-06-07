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
#define SUCCESS 1
#define FAILURE 0
#define EFD -3
#define ESIZE -4
#define SUPERBLOCKADDR 0
#define ROOTINODEADDR 1
#define EREAD -5
#define EFILENOTFOUND -6
#define EINVALID -7
#define EOUTOFMEM -8
#define EWRITE -9
#define DATABLOCKSIZE 253

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

typedef struct freeblock{
	uint8_t blocktype;
	uint8_t magic_number;
	uint8_t next_block;
	uint8_t buffer[DATABLOCKSIZE];
}__attribute__((packed)) freeblock;

typedef struct inodeblock{
	uint8_t blocktype;
	uint8_t magic_number;
	uint8_t next_inode;
	char filename [9];
	uint32_t file_size;
	uint8_t file_pointer;
	uint8_t buffer[BLOCKSIZE-17];
}__attribute__((packed)) inodeblock;

typedef struct superblock{
	uint8_t blocktype;
	uint8_t magic_number;
	uint8_t root_inode;
	uint8_t free_block_pointer;
	uint8_t free_block_count;
	uint8_t buffer[BLOCKSIZE-6];
}__attribute__((packed)) superblock;


typedef struct fdNode {
	int data;
	uint8_t blockNbr;
	uint64_t offset;
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
int locateFile(char *name);
int newFile(char *name);
uint8_t locateLastInode();
fileblock newFileBlock(uint8_t next_block_offset);
inodeblock newInode(char *name, uint8_t fp);
int tfs_seek(fileDescriptor FD, int offset);
fdNode* create(int data, fdNode* next, uint8_t blockNbr, uint64_t offset);
fdNode* add(fdNode* head, int data, uint8_t blockNbr, uint64_t offset);
int removeNode(fdNode* head, int data);
int containsFD(fdNode* head, int fd);
int resetOffset(int fd);
int setOffset(int fd, int offset);
int getBlockNbr(fdNode* head, int fd);
void clearBlocks(int first_block);
int getFreeBlocks(int nbr, int index_free, int);
int writeDataToFiles(int blockNbr, int head, int size, uint8_t* buffer);
int blockSize(int size);
int getOffsetBlock(int head, int offset);
int getOffset(int fd);

#endif
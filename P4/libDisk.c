#include "libDisk.h"
#include "tinyFS.h"

int openDisk(char *filename, int nBytes) {
	int fd;

	if (nBytes % BLOCKSIZE != 0) {
		return NBYTES_ERR
	}
	if (nBytes == 0 && access(filename, F_OK) == -1) {
		return ERR_ACCESS_FILE
	}
	if ((fd = open(filename, O_RDWR | O_CREAT, S_IRUSR, S_IWUSR, S_IRGRP, S_IWGRP, S_IROTH)) == -1) {
		return -1
	}
	if (nBytes > 0 && ftruncate(fd, nBytes) == -1) {
		close(fd);
		return ERR_RESIZE	
	}

	return fd;
}

int readBlock(int disk, int bNum, void *block) {
	if (lseek(disk, bNum * BLOCKSIZE, SEEK_SET) == -1) {
		return -1;
	}
	if (read(disk, block, BLOCKSIZE) == -1) {
		return -1;
	} 

	return 0;
}

int writeBlock(int disk, int bNum, void *block) {
	if (lseek(disk, bNum * BLOCKSIZE, SEEK_SET) == -1) {
		return -1;
	}
	if (write(disk, block, BLOCKSIZE) == -1) {
		return -1;
	} 

	return 0;
}

void closeDisk(int disk) {
	close(disk);
}

#include "frontEnd.h"

int testSuite1(char * fs_name);
void printSuperInfo();

int main(){
	int fs;
	tfs_mkfs("temp",10240);
	testSuite1("temp");
	return 0;
}

int testSuite1(char * fs_name){
	int i;
	int fd;
	int fd2;
	int fd3;
	char first_string[983];
	char second_string[583];
	for(i = 0; i < 983; i++){
		first_string[i] = 'e';
	}
	for(i = 0; i < 583; i++){
		second_string[i] = 'b';
	}
	printf("Mounting filesystem: %s...\n",fs_name);
	tfs_mount(fs_name);
	printf("Opening file: file1\n\n");
	fd = tfs_openFile("file1");
	printf("Opening file: file2\n\n");
	fd2 = tfs_openFile("file2");
	printf("writing string to file1 with size %d\n\n",(int)strlen(first_string));
	tfs_writeFile(fd, first_string, strlen(first_string));
	printf("writing string to file2 with size %d\n\n",(int)strlen(second_string));
	tfs_writeFile(fd2, second_string, strlen(second_string));
	printf("Opening file: file3\n\n");
	fd3 = tfs_openFile("file3");
	printf("writing string to file3 with size %d\n\n",(int)strlen(first_string));
	tfs_writeFile(fd3, first_string, strlen(first_string));
	printSuperInfo();
	printf("Unmounting disk...\n");
	tfs_unmount();
	return 0;
}

void printSuperInfo(){
	superblock root;

	readBlock(diskNO, SUPERBLOCKADDR, &root); 

	printf("Superblock data:\n");
	printf("-Next Free Block: %d\n",root.free_block_pointer);
	printf("-Remaining Free blocks: %d\n", root.free_block_count);
	printf("-Directory Files:\n");
	tfs_readdir();
}
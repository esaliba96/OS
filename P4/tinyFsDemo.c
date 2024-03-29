#include "tinyFsDemo.h"

int main(){
	int fs;
	tfs_mkfs("temp",10240);
	printf("running test suite 1...\n\n");
	testSuite1("temp");
	printf("Waiting for 1 minute...\n\n");
	sleep(60);
	printf("running test suite 2...\n\n");
	testSuite2("temp");
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
	printf("Running TestSuite 1:\n\n");
	printf("Mounting filesystem: %s...\n\n",fs_name);
	tfs_mount(fs_name);
	printf("Opening file: file1\n\n");
	fd = tfs_openFile("file1");
	printf("Opening file: file2\n\n");
	fd2 = tfs_openFile("file2");

	printf("writing string to file1 with size %d\n\n",(int)strlen(first_string));
	printf("file1 info:\n");
	tfs_writeFile(fd, first_string, strlen(first_string));
	printInodeInfo("file1");
	
	printf("writing string to file2 with size %d\n\n",(int)strlen(second_string));
	printf("file2 info:\n");
	tfs_writeFile(fd2, second_string, strlen(second_string));
	printInodeInfo("file2");
	
	printf("Opening file: file3\n\n");
	fd3 = tfs_openFile("file3");
	printf("writing string to file3 with size %d\n\n",(int)strlen(first_string));
	printf("file3 info:\n");
	tfs_writeFile(fd3, first_string, strlen(first_string));
	printInodeInfo("file3");

	printSuperInfo();
	printf("\nUnmounting disk...\n\n");
	tfs_unmount();
	return 0;
}

int testSuite2(char *fs_name){
	int fd1,fd2,fd3;
	char first_string[983];
	char list[1];

	printf("-------------------\n\nRunning TestSuite2:\n\n");
	printf("Mounting Filesystem temp...\n\n");
	tfs_mount(fs_name);
	fd1 = tfs_openFile("file1");
	fd2 = tfs_openFile("file2");
	fd3 = tfs_openFile("file3");

	printSuperInfo();

	printf("\nDeleting File: file1\n\n");
	tfs_deleteFile(fd1);

	printf("Testing Seek, Read and Write:\n\n");
	printf("Seeking to address: 37 in file2\n");
	tfs_seek(fd2, 37);
	printf("Writing Character: W\n\n");
	tfs_writeByte(fd2, 'W');
	printf("Seeking to address: 243 in file2\n");
	tfs_seek(fd2, 243);
	printf("Writing Character: Z\n\n");
	tfs_writeByte(fd2, 'Z');
	printInodeInfo("file2");
	printf("Seeking to address: 89 in file3\n");
	tfs_seek(fd3, 89);
	printf("Writing Character: P\n\n");
	tfs_writeByte(fd3, 'P');
	printf("Seeking to address: 383 in file2\n");
	tfs_seek(fd3, 383);
	printf("Writing Character: Q\n\n");
	tfs_writeByte(fd3, 'Q');
	printInodeInfo("file3");

	printf("Making file2 Read-Only...\n\n");
	tfs_makeRO("file2");
	printInodeInfo("file2");
	printf("Attempting to write to file2...\n");
	tfs_writeByte(fd2, 'Q');
	printf("\nMaking file2 Read/Write...\n\n");
	tfs_makeRW("file2");
	printInodeInfo("file2");

	tfs_seek(fd2, 37);
	tfs_readByte(fd2,list);
	printf("Reading Byte: %c from offset 37\n\n",list[0]);
	
	tfs_seek(fd2, 243);
	tfs_readByte(fd2,list);
	printf("Reading Byte: %c from offset 243\n\n",list[0]);
	printInodeInfo("file2");
	tfs_seek(fd3, 89);
	tfs_readByte(fd3,list);
	printf("Reading Byte: %c from offset 89\n\n",list[0]);

	tfs_seek(fd3, 383);
	tfs_readByte(fd3,list);
	printf("Reading Byte: %c from offset 383\n\n",list[0]);
	printInodeInfo("file3");

	printf("Renaming file2 to file4\n\n");
	tfs_rename(fd2, "file4");
	printInodeInfo("file4");
	printSuperInfo();

	printf("Deleting file4\n\n");
	tfs_deleteFile(fd2);
	printf("Deleting file3\n\n");
	tfs_deleteFile(fd3);

	printSuperInfo();
	return 0;
}

void printSuperInfo(){
	superblock root;

	readBlock(diskNO, SUPERBLOCKADDR, &root); 

	printf("Superblock data:\n");
	printf("-Next Free Block: %d\n",root.free_block_pointer);
	printf("-Remaining Free blocks: %d\n\n", root.free_block_count);
	printf("-Directory Files:\n");
	tfs_readdir();
}

void printInodeInfo(char* filename){
	inodeblock inode;
	readBlock(diskNO,hasFile(filename),&inode);
	printf("File Info for: %s\n",filename);
	printf("\tFile Name : %s\n",filename);
	printf("\tSize: %d\n",inode.file_size);
	if(inode.RO == 1){
		printf("\tPermissions: R\n");
	} else{
		printf("\tPermissions: RW\n");
	}
	struct tm tm = *localtime(&inode.cTime);
	printf("\tcreated: %d-%d-%d %d:%d:%d\n", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
	struct tm tm1 = *localtime(&inode.aTime);
	printf("\taccessed: %d-%d-%d %d:%d:%d\n", tm1.tm_year + 1900, tm1.tm_mon + 1, tm1.tm_mday, tm1.tm_hour, tm1.tm_min, tm1.tm_sec);
	tm = *localtime(&inode.mTime);
	printf("\tmodified: %d-%d-%d %d:%d:%d\n\n", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
}
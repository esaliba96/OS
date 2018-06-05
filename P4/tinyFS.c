#include "tinyFS.h"

fdNode* head = NULL;

fdNode* create(int data, fdNode* next, uint8_t blockNbr) {
    fdNode* new_node = (fdNode*)malloc(sizeof(fdNode));
    
    if(new_node == NULL) {
        printf("Error creating a new node.\n");
        exit(0);
    }
    new_node->data = data;
    new_node->next = next;
    new_node->blockNbr = blockNbr;
 
    return new_node;
}

fdNode* add(fdNode* head, int data, uint8_t blockNbr) {
 	fdNode* new_node = create(data, head, blockNbr);
    head = new_node;
    
    return head;
}

int removeNode(fdNode* head, int data) {
	fdNode *curr, *prev;
  	prev = NULL;

  	for (curr = head; curr != NULL; prev = curr, curr = curr->next) {
	    if (curr->data == data) {  
	    	if (prev == NULL) {
	        	head = curr->next;
	      	} else {
	        	prev->next = curr->next;
	      	}
			free(curr);
	     	return SUCCESS;
	    }
  	}

  	return FAILURE;
}

int containsFD(fdNode* head, int fd) {
	while(head != NULL) {
		if (head->data == fd) {
			return 1;
		} 
		head = head->next;
	}
	return 0;
}

int getBlockNbr(fdNode* head, int fd) {
	while (head != NULL) {
		if (head->data == fd) {
			return head->blockNbr;
		}
		head = head->next;
	}

	return -1;
}

int tfs_mkfs(char *filename, int nBytes){
	uint8_t init= 0x0;
	int fs;
	int i;
	int check;
	int num_blocks = nBytes/BLOCKSIZE;
	block init_block;
	block block_list[num_blocks];

	
	//printf("%i\n",num_blocks);
	//Initialize file system with all zeroes
	fs = openDisk(filename,nBytes);
	if(fs == NBYTES_ERR){
		printf("Invalid Filesystem System Size, must be divisible by 256\n");
		return -1; //unable to open and configure file
	} else if(fs == ERR_ACCESS_FILE){
		printf("Unable to access file %s\n", filename);
		return -1;
	} else if(fs == ERR_RESIZE){
		printf("Unable to truncate to filesystem length\n");
		return -1;
	}

	for(i=0;i<num_blocks;i++){
		if(i<2){
			init_block = init_blocks(i+1);
			if(i == 0){
				init_block.buffer[4] = num_blocks - 2;
			}
		} else{
			init_block = init_blocks(i+1);
			init_block.buffer[2] = i+1;
		}
		if(writeBlock(fs,i,&init_block) == -1){
			printf("Disk Write Error\n");
			return -1;
		}
	}
	closeDisk(fs);
	return 0;

}

int tfs_mount(char *filename){
	//initialize file descriptor memory here
	diskNO = openDisk(filename,0);
	if(verify_fs() == -1){
		return -1;
	}
	return diskNO;
}

int tfo_unmount(void){
	//force close all open file descriptors
	closeDisk(diskNO);
	return 0;
}

int verify_fs(void){
	block to_verify;
	int i = 0;
	int test=0;
	test = readBlock(diskNO,i++,&to_verify);
	int end = to_verify.buffer[4];
	while(i<end){
		test = readBlock(diskNO,i++,&to_verify);
		if(to_verify.buffer[1] != 0x45){
			return -1;
		}
	}
	return 0;
}

block init_blocks(int type){
	block to_return;
	memset(&to_return,0,BLOCKSIZE);

	if(type == 1){ // super block
		to_return.buffer[0] = 0x01;
		to_return.buffer[2] = 0x01;
		to_return.buffer[3] = 0x02;
	} else if(type == 2){ // inode block
		to_return.buffer[0] = 0x02;
		to_return.buffer[2] = 0x00;
	} else if(type == 3){ //file block
		to_return.buffer[0] = 0x03;
	} else{
		to_return.buffer[0] = 0x04;
	}

	to_return.buffer[1] = 0x45;
	return to_return;
}


int tfs_openFile(char *name){
	static int file_descriptor_bucket;
	inodeblock new_inode;

	int block_no;
	if((block_no = locateFile(name))!=EFILENOTFOUND){
		head = add(head,file_descriptor_bucket,block_no);
		return file_descriptor_bucket++;
	} 
	block_no = newFile(name);
	if(block_no < 0){
		return block_no;
	}
	head = add(head,file_descriptor_bucket,block_no);
	return file_descriptor_bucket++;

}

int newFile(char *name){
	int err;
	superblock root;
	freeblock for_inode;
	inodeblock new_block;
	inodeblock last_block;

	uint8_t inode_block_no;
	uint8_t current_last_inode = locateLastInode();

	if(readBlock(diskNO, SUPERBLOCKADDR, &root) == EREAD){ // read superblock
		return EINVALID;
	}
	if(readBlock(diskNO, current_last_inode, &last_block) == EREAD){ // read superblock
		return EINVALID;
	}
	if(root.free_block_count < 2){ // check available mem
		return EOUTOFMEM;
	}
	if(readBlock(diskNO, root.free_block_pointer, &for_inode) == EREAD){ // readblock for inode
		return EINVALID;
	}
	inode_block_no = root.free_block_pointer; // grab inode block number
	root.free_block_pointer = for_inode.next_block; // update freeblockpointer

	last_block.next_inode = inode_block_no; // update the current last inode for write
	new_block = newInode(name,0); ///build new inode for write
	root.free_block_count -=1;

	if(writeBlock(diskNO, SUPERBLOCKADDR, &root) == -1){
			printf("Disk Write Error\n");
			return EWRITE;
	}
	if(writeBlock(diskNO, current_last_inode ,&last_block) == -1){
			printf("Disk Write Error\n");
			return EWRITE;
	}
	if(writeBlock(diskNO, inode_block_no, &new_block) == -1){
			printf("Disk Write Error\n");
			return EWRITE;
	}
	return inode_block_no;
}

inodeblock newInode(char *name, uint8_t fp){
	inodeblock to_return;
	memset(&to_return,0,BLOCKSIZE);
	to_return.blocktype = 0x02;
	to_return.magic_number = 0x45;
	to_return.next_inode = 0x00;
	strcpy(to_return.filename,name);
	to_return.file_size = 0;
	to_return.file_pointer = fp;
	return to_return;
 }

fileblock newFileBlock(uint8_t next_block_offset){
	fileblock to_return;
	memset(&to_return,0,BLOCKSIZE);
	to_return.blocktype = 0x03;
	to_return.magic_number = 0x45;
	to_return.next_block = next_block_offset;
	return to_return;
}

uint8_t locateLastInode(){
	int err;
	superblock root;
	inodeblock searcher;
	uint8_t block_no;

	err = readBlock(diskNO, SUPERBLOCKADDR, &root);

	block_no = root.root_inode;
	err = readBlock(diskNO, block_no, &searcher);
	while(searcher.next_inode != 0x00){
		block_no = searcher.next_inode;
		err = readBlock(diskNO, block_no, &searcher);
	}
	return block_no;
}

int locateFile(char *name){
	int err;
	uint8_t block_no;
	superblock root;
	inodeblock searcher;

	err = readBlock(diskNO, SUPERBLOCKADDR, &root); 
	if(err == -1){
		return EREAD;
	}

	block_no = root.root_inode;
	err = readBlock(diskNO, block_no, &searcher);

	while(searcher.next_inode != 0x00){
		if(strcmp(name,searcher.filename) == 0){
			return block_no;
		}
		block_no = searcher.next_inode;
		err = readBlock(diskNO, block_no, &searcher);
	}
	return EFILENOTFOUND;
}

int main(){
	int fs;
	tfs_mkfs("temp",10240);
	printf("%i\n",tfs_mount("temp"));
	//printf("%i\n",tfo_unmount());

	// head = add(head, 1, 7);
	// head = add(head, 2, 8);
	// head = add(head, 3, 4);
	// head = add(head, 4, 42);
	// head = add(head, 5, 42);
	// head = add(head, 6, 32);

	fs = tfs_openFile("fucker");
	
	// while (head) {
	// 	printf("%d\n", head->blockNbr);
	// 	head = head->next;
	// }

	tfs_writeFile(fs, "hello", 955);

	return 0;
}

int tfs_closeFile(fileDescriptor FD){
	if (removeNode(head, FD)) {
		return SUCCESS;
	}
	return EFD;
}

int tfs_writeFile(fileDescriptor FD, char *buffer, int size) {
	if (!containsFD(head, FD)) {
		return EFD;
	}

	if (size <= 0) {
		return ESIZE;
	}

	block b;

	if (readBlock(diskNO, SUPERBLOCKADDR, &b) == -1) {
		return EREAD;
	}

	superblock* s = (superblock*)(b.buffer);
	int free_blocks = s->free_block_count;
	int block_nbr = getBlockNbr(head, FD);
	printf("block nbr %d\n", block_nbr);
	int free = s->free_block_pointer;
	printf("block %d\n", free);

	if (readBlock(diskNO, block_nbr, &b) == -1) {
		return EREAD;
	} 

	inodeblock* inode = (inodeblock*)(b.buffer);
	inode->file_pointer = free;
	printf("inode %d\n", inode->file_pointer);

	if (writeBlock(diskNO, block_nbr, &inode) == 1) {
		printf("error writing in tfs_writeFile\n");
		return EWRITE;
	}

	int freeMem = inode->file_size % BLOCKSIZE + free_blocks * BLOCKSIZE;

	if (size > freeMem) {
		return EOUTOFMEM;
	}

	int blocksNeeded;

	if (size % BLOCKSIZE != 0) {
		blocksNeeded = size/BLOCKSIZE + 1; 
	} else {
		blocksNeeded = size/BLOCKSIZE;
	}

	getFreeBlocks(blocksNeeded, free, free_blocks);

	printf("inode %d\n", inode->file_size);

	return SUCCESS;
}

int getFreeBlocks(int nbr, int index_free, int free_blocks) {
	int i = 0;
	block b;
	freeblock* free;
	fileblock file;
	superblock root;

	for (; i < nbr; i++) {
		if (readBlock(diskNO, index_free, &b) == -1) {
			return EREAD;
		}
		free = (freeblock*) (b.buffer);
		printf("next %d\n", free->next_block);
		file = newFileBlock(free->next_block);

		if (writeBlock(diskNO, index_free, &file) == 1) {
			return EWRITE;
		}

		index_free =  free->next_block;
		free_blocks--;
	}

	if(readBlock(diskNO, SUPERBLOCKADDR, &root) == EREAD){ // read superblock
		printf("error reading in getFreeBlocks\n");
		return EINVALID;
	}

	root.free_block_count = free_blocks;

	if (writeBlock(diskNO, SUPERBLOCKADDR, &root) == 1) {
		printf("error writing in getFreeBlocks\n");
		return EWRITE;
	}

	printf("free blocks %d\n", free_blocks);
}
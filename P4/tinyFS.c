#include "tinyFS.h"

fdNode* head = NULL;

fdNode* create(int data, fdNode* next, uint8_t blockNbr, uint64_t offset) {
    fdNode* new_node = (fdNode*)malloc(sizeof(fdNode));
    
    if(new_node == NULL) {
        printf("Error creating a new node.\n");
        exit(0);
    }
    new_node->data = data;
    new_node->next = next;
    new_node->blockNbr = blockNbr;
    new_node->offset = offset;
 
    return new_node;
}

fdNode* add(int data, uint8_t blockNbr, uint64_t offset) {
 	fdNode* new_node = create(data, head, blockNbr, offset);
    head = new_node;
    
    return head;
}

int removeNode(int data) {
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

int containsFD(int fd) {
	while(head != NULL) {
		if (head->data == fd) {
			return 1;
		} 
		head = head->next;
	}
	return 0;
}

int resetOffset(int fd){
	while(head != NULL) {
		if (head->data == fd) {
			head->offset = 0;
			return 1;
		} 
		head = head->next;
	}
	return 0;
}

int setOffset(int fd, int offset){
	while(head != NULL) {
		if (head->data == fd) {
			head->offset = offset;
			return 1;
		} 
		head = head->next;
	}
	return 0;
}

int getOffset(int fd) {
	while(head != NULL) {
		if (head->data == fd) {
			return head->offset;
		} 
		head = head->next;
	}
	return 0;
}

int getBlockNbr(int fd) {
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
		head = add(file_descriptor_bucket,block_no,0);
		return file_descriptor_bucket++;
	} 
	block_no = newFile(name);
	if(block_no < 0){
		return block_no;
	}
	head = add(file_descriptor_bucket,block_no,0);
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

int tfs_deleteFile(fileDescriptor FD){
	int err;
	superblock root;
	inodeblock inode;
	inodeblock iterator; //NW
	freeblock current_last; //NW
	int file_block_no;
	int pointer_to_inode;
	int files_to_return;
	int last_free_block;

	err = containsFD(FD);
	if(err == 0){
		return EFILENOTFOUND;
	}

	if((err = readBlock(diskNO, SUPERBLOCKADDR, &root))== -1){ // get superblock
		return EREAD;
	}

	pointer_to_inode = root.root_inode;
	file_block_no = getBlockNbr(FD);
	
	if((readBlock(diskNO, pointer_to_inode, &iterator)) == -1){//get inode of file to be deleted
		return EREAD;
	}

	while(iterator.next_inode!=file_block_no){//locate the inode pointing to the inode that will be deleted
		if((readBlock(diskNO, iterator.next_inode, &iterator)) == -1){
			return EREAD;
		}
		if(iterator.next_inode!=file_block_no){
			pointer_to_inode = iterator.next_inode;
		}
	}

	if((readBlock(diskNO, file_block_no, &inode)) == -1){//get inode of file to be deleted
		return EREAD;
	}
	iterator.next_inode = inode.next_inode; // removing the inode from the inode list
	last_free_block = getLastFreeBlock();
	
	if((readBlock(diskNO, last_free_block, &current_last)) == -1){ //get the current last freeblock in the list
		return EREAD;
	}

	current_last.next_block = file_block_no;
	files_to_return = countBlocks(file_block_no); // find the number of blocks to be freed
	clearBlocks(file_block_no,files_to_return); // clear and free the blocks including the inode
	root.free_block_count += files_to_return; //increment the number of free blocks in root

	if (writeBlock(diskNO, SUPERBLOCKADDR, &root) == 1) {
		return EWRITE;
	}

	if (writeBlock(diskNO, pointer_to_inode, &iterator) == 1) {
		return EWRITE;
	}

	if (writeBlock(diskNO, last_free_block, &current_last) == 1) {
		return EWRITE;
	}
	return 0;
}

int countBlocks(int inode_addr){
	fileblock counter;
	inodeblock inode;
	int count = 1;
	int addr = inode_addr;
	
	if((readBlock(diskNO, inode_addr, &inode)) == -1){//get inode of file to be deleted
		return EREAD;
	}
	addr = inode.file_pointer;
	do{
		if((readBlock(diskNO, addr, &counter)) == -1){//get inode of file to be deleted
			return EREAD;
		}
		addr = counter.next_block;
		if(counter.blocktype == 0x03){
			count++;
		}
	}while(counter.blocktype == 0x03);
	return count;
}

void clearBlocks(int first_block, int to_clear){
	fileblock to_clear_block;
	inodeblock temp;
	int next = first_block;
	int temp_addr;

	while(to_clear > 0){
		if((readBlock(diskNO, next, &to_clear_block)) == -1){ //get the current last freeblock in the list
			return;
		}
		if(to_clear_block.blocktype == 0x02){
			to_clear_block.next_block = to_clear_block.buffer[13];
		}
		memset(&to_clear_block.buffer,0,253);
		to_clear_block.blocktype = 0x04;
		if (writeBlock(diskNO, next, &to_clear_block) == 1) {
			printf("error writing in tfs_writeFile\n");
			return;
		}
		next = to_clear_block.next_block;
		to_clear--;
	}

}

int getLastFreeBlock(){
	superblock root;
	freeblock current;
	freeblock new;
	int err;
	uint8_t counter;
	uint8_t next;

	if((err = readBlock(diskNO, SUPERBLOCKADDR, &root))== -1){
		return EREAD;
	}
	if(root.free_block_count <= 0){
		return 0;
	}
	counter = root.free_block_count - 1;

	if((err = readBlock(diskNO, root.free_block_pointer, &current))== -1){
		return EREAD;
	}
	counter--;
	while(counter > 0){
		if((err = readBlock(diskNO, current.next_block, &current))== -1){
			return EREAD;
		}
		counter--;
	}
	return current.next_block;
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

	tfs_writeFile(fs, "lllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllll", 639);

	//tfs_writeFile(fs, "hello", 955);
	//tfs_deleteFile(fs);
	printf("name: %s\n", tfs_rename(fs, "hello"));
	//printf("address of last block %d\n", getLastFreeBlock());

	return 0;
}

int tfs_closeFile(fileDescriptor FD){
	if (removeNode(FD)) {
		return SUCCESS;
	}
	return EFD;
}

int tfs_writeFile(fileDescriptor FD, char *buffer, int size) {
	superblock root;
	inodeblock inode;
	int free_blocks;
	int block_nbr;
	int free;

	if (!containsFD(FD)) {
		return EFD;
	}

	if (size <= 0) {
		return ESIZE;
	}

	block b;

	if (readBlock(diskNO, SUPERBLOCKADDR, &root) == -1) {
		return EREAD;
	}

	free_blocks = root.free_block_count;
	block_nbr = getBlockNbr(FD);
	
	printf("block nbr %d\n", block_nbr);
	
	free = root.free_block_pointer;
	printf("block %d\n", free)	;

	if (readBlock(diskNO, block_nbr, &inode) == -1) {
		return EREAD;
	} 

	inode.file_pointer = free;
	int freeMem = inode.file_size % DATABLOCKSIZE + free_blocks * DATABLOCKSIZE;
	printf("inode %d\n", inode.file_pointer);
	inode.file_size += size;

	if (writeBlock(diskNO, block_nbr, &inode) == 1) {
		printf("error writing in tfs_writeFile\n");
		return EWRITE;
	}

	if (size > freeMem) {
		return EOUTOFMEM;
	}

	int blocks = blockSize(size); 
	
	int head = getFreeBlocks(blocks, free, free_blocks);

	writeDataToFiles(blocks, head, size, buffer);

	resetOffset(FD);
	printf("inode %d\n", inode.file_size);

	return SUCCESS;
}

int getFreeBlocks(int nbr, int index_free, int free_blocks) {
	int i = 0;
	block b;
	freeblock* free;
	fileblock file;
	superblock root;
	int headFreeBlock = index_free;

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

		index_free = free->next_block;
		free_blocks--;
	}

	if(readBlock(diskNO, SUPERBLOCKADDR, &root) == EREAD){ // read superblock
		printf("error reading in getFreeBlocks\n");
		return EINVALID;
	}

	root.free_block_count = free_blocks;
	root.free_block_pointer = index_free;

	if (writeBlock(diskNO, SUPERBLOCKADDR, &root) == 1) {
		printf("error writing in getFreeBlocks\n");
		return EWRITE;
	}

	printf("free blocks %d\n", free_blocks);

	return headFreeBlock;
}

int blockSize(int size) {
	int blockSize;

	if (size % DATABLOCKSIZE != 0) {
		blockSize = size/DATABLOCKSIZE + 1; 
	} else {
		blockSize = size/DATABLOCKSIZE;
	}

	return blockSize;
}

int writeDataToFiles(int blockNbr, int head, int size, uint8_t* buffer) {
	int i = 0;
	fileblock file;
	
	for (; i < blockNbr - 1; i++) {
		if (readBlock(diskNO, head, &file) == -1) {
			return EREAD;
		}
		memcpy(file.buffer, buffer + (i * BLOCKSIZE), DATABLOCKSIZE);

		if (writeBlock(diskNO, head, &file) == 1) {
			printf("error writing in getFreeBlocks\n");
			return EWRITE;
		}
		head = file.next_block;
	}

	int remaining = size % DATABLOCKSIZE;

	if (readBlock(diskNO, head, &file) == -1) {
		return EREAD;
	}
	memcpy(file.buffer, buffer + (i * DATABLOCKSIZE), remaining);

	if (writeBlock(diskNO, head, &file) == 1) {
		printf("error writing in getFreeBlocks\n");
		return EWRITE;
	}

	return SUCCESS;
}

int tfs_seek(fileDescriptor FD, int offset) {
	inodeblock inode;
	int block_nbr;

	if (!containsFD(FD)) {
		return EFD;
	}

	block_nbr = getBlockNbr(FD);
	if (readBlock(diskNO, block_nbr, &inode) == -1) {
		return EREAD;
	} 

	if (blockSize(inode.file_size) >= blockSize(offset)) {
		setOffset(FD, offset);
		return SUCCESS;
	}
}

int tfs_readByte(fileDescriptor FD, char *buffer) {
	inodeblock inode;
	int block_nbr;
	fileblock file;

	if (!containsFD(FD)) {
		return EFD;
	}

	block_nbr = getBlockNbr(FD);

	if (readBlock(diskNO, block_nbr, &inode) == -1) {
		return EREAD;
	} 

	int offset = getOffset(FD);
	int dataBlock = getOffsetBlock(inode.file_pointer, offset);

	if (readBlock(diskNO, dataBlock, &file) == -1) {
    	return EREAD;
   	}

   *buffer = file.buffer[offset % DATABLOCKSIZE];
   	offset++;
  	setOffset(FD, offset);

   	return SUCCESS;
}

int getOffsetBlock(int head, int offset) {
	fileblock file;
	int i = 0 , block_nbr = offset / DATABLOCKSIZE;

   	for (; i < block_nbr; i++) {
    	if (readBlock(diskNO, head, &file) == -1) {
        	return EREAD;
      	}

      	head = file.next_block;
   }

   return head;
}

int tfs_rename(int FD, char* new_name) {
	inodeblock inode;
	int nbr = getBlockNbr(FD);

	if (readBlock(diskNO, nbr, &inode) == -1) {
		return EREAD;
	} 

	printf("%s\n", inode.filename);
}
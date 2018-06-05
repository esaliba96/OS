#include "tinyFS.h"

fdNode* create(int data, fdNode* next) {
    fdNode* new_node = (fdNode*)malloc(sizeof(fdNode));
    
    if(new_node == NULL) {
        printf("Error creating a new node.\n");
        exit(0);
    }
    new_node->data = data;
    new_node->next = next;
 
    return new_node;
}

fdNode* add(fdNode* head, int data) {
 	fdNode* new_node = create(data,head);
    head = new_node;
    
    return head;
}

void removeNode(fdNode* head, int data) {
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
     	return;
    }
  }
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
		if(i<3){
			init_block = init_blocks(i+1);
			if(i == 0){
				init_block.buffer[4] = num_blocks - 2;
			}
		} else{
			init_block = init_blocks(i+1);
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
		to_return.buffer[2] = 0x02;
		to_return.buffer[3] = 0x03;
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

int main(){
	int fs;
	tfs_mkfs("temp",10240);
	//printf("%i\n",tfs_mount("temp"));
	//printf("%i\n",tfo_unmount());

	fdNode* head = NULL;

	head = add(head, 1);
	head = add(head, 2);
	head = add(head, 3);
	removeNode(head, 2);
	head = add(head, 4);
	head = add(head, 5);
	head = add(head, 6);
	removeNode(head, 1);

	while (head) {
		printf("%d\n", head->data);
		head = head->next;
	}
	return 0;
}

int tfs_mkfs(char *filename, int nBytes){
	// uint8_t init= 0x0;
	// int fs;
	// int i;
	// int check;
	// int num_blocks = nBytes/BLOCKSIZE;
	// block init_block;
	// block block_list[num_blocks];

	
	// //printf("%i\n",num_blocks);
	// //Initialize file system with all zeroes
	// fs = openDisk(filename,nBytes);
	// if(fs == NBYTES_ERR){
	// 	printf("Invalid Filesystem System Size, must be divisible by 256\n");
	// 	return -1; //unable to open and configure file
	// } else if(fs == ERR_ACCESS_FILE){
	// 	printf("Unable to access file %s\n", filename);
	// 	return -1;
	// } else if(fs == ERR_RESIZE){
	// 	printf("Unable to truncate to filesystem length\n");
	// 	return -1;
	// }

	// for(i=0;i<num_blocks;i++){
	// 	if(i<3){
	// 		init_block = init_blocks(i+1);
	// 		if(i == 0){
	// 			init_block.buffer[4] = num_blocks - 2;
	// 		}
	// 	} else{
	// 		init_block = init_blocks(i+1);
	// 	}
	// 	if(writeBlock(fs,i,&init_block) == -1){
	// 		printf("Disk Write Error\n");
	// 		return -1;
	// 	}
	// }
	// closeDisk(fs);
	// return 0;
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
		if(i<3){
			init_block = init_blocks(i+1);
			if(i == 0){
				init_block.buffer[4] = num_blocks - 2;
			}
		} else{
			init_block = init_blocks(i+1);
		}
		if(writeBlock(fs,i,&init_block) == -1){
			printf("Disk Write Error\n");
			return -1;
		}
	}
	closeDisk(fs);
	return 0;

}
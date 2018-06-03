#include "TinyFS.h"


int tfs_mkfs(char *filename, int nBytes){
	uint8_t init= 0x0;
	int fs;
	int i;
	int check;
	int num_blocks = nBytes/BLOCKSIZE;
	block init_block;
	block block_list[num_blocks];

	
	printf("%i\n",num_blocks);
	//Initialize file system with all zeroes
	fs = open(filename, O_CREAT | O_RDWR | S_IRUSR);
	if(fs == -1){
		return -1; //unable to open and configure file
	}
	for(i = 0; i<nBytes; i++){
		if(write(fs,&init,1) == -1){
			return -2; //invalid space requirements
		}	
	}
	for(i=0;i<num_blocks;i++){
		if(i>0 && i<3){
			init_block = init_blocks(i);
		} else{
			init_block = init_blocks(0);
		}
		lseek(fs,i*BLOCKSIZE,SEEK_SET);
		write(fs,init_block.buffer,BLOCKSIZE);
	}
	close(fs);

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
	tfs_mkfs("temp",10240);
	return 0;
}

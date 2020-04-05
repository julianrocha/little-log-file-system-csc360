#include <stdio.h>
#include <stdlib.h>
#include "disk_controller.h"

FILE* disk_pointer;

void write_block_to_disk(unsigned char block[], int index){
	fseek(disk_pointer, index * BLOCK_SIZE, SEEK_SET);
	fwrite(block, BLOCK_SIZE, 1, disk_pointer);
}

void read_block_from_disk(unsigned char block[], int index){
	fseek(disk_pointer, index * BLOCK_SIZE, SEEK_SET);
	fread(block, BLOCK_SIZE, 1, disk_pointer);
}

void create_fresh_disk(){
	disk_pointer = fopen(DISK_PATH, "wb+");
	char* init = calloc(BLOCK_SIZE * NUM_BLOCKS, 1);
	fwrite(init, BLOCK_SIZE * NUM_BLOCKS, 1, disk_pointer);
	free(init);
}

void attach_disk(){
	disk_pointer = fopen(DISK_PATH, "rb+");
	if(disk_pointer == NULL){
		create_fresh_disk();
	}
}

void wipe_disk(){
	remove(DISK_PATH);
}
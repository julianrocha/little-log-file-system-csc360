#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../disk/disk_controller.h"
#include "../io/File.h"

void test01_wipe_and_attach_disk(){
	wipe_disk();
	FILE* test_fp = fopen(DISK_PATH, "rb+");
	if(test_fp != NULL){
		printf("test01 failed\n");
		return;
	}
	attach_disk();
	test_fp = fopen(DISK_PATH, "rb+");
	if(test_fp == NULL){
		printf("test01 failed\n");
		return;
	}
	printf("test01 passed\n");
	wipe_disk();
}

void test02_write_and_read_blocks(){
	attach_disk();
	char block1[BLOCK_SIZE];
	memset(block1, 120, BLOCK_SIZE);
	write_block_to_disk(block1, 0);
	write_block_to_disk(block1, 2);
	char block2[BLOCK_SIZE];
	memset(block2, 0, BLOCK_SIZE);
	char block3[BLOCK_SIZE];
	read_block_from_disk(block3, 0);
	if(strncmp(block1, block3, BLOCK_SIZE)){
    	printf("test02 failed\n");
    	return;
    } 
    read_block_from_disk(block3, 1);
	if(strncmp(block2, block3, BLOCK_SIZE)){
    	printf("test02 failed\n");
    	return;
    }
    read_block_from_disk(block3, 2);
	if(strncmp(block1, block3, BLOCK_SIZE)){
    	printf("test02 failed\n");
    	return;
    }
    printf("test02 passed\n");
}


int main(){
	printf("BEGIN TEST SUITE\n");
	test01_wipe_and_attach_disk();
	test02_write_and_read_blocks();
	printf("END OF TEST SUITE\n");
}

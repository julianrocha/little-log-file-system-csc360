#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../disk/disk_controller.h"
#include "./File.h"

#define MAGIC 42
#define NUM_I_NODES 256 // ID of i-node must occupy 1 byte
#define I_NODE_SIZE 32
#define DIR_FILE 0
#define DATA_FILE 1
#define SEG_BLOCKS 8

unsigned char segment_buffer[SEG_BLOCKS * BLOCK_SIZE];
unsigned char i_node_map[NUM_I_NODES * 2];
unsigned char free_vector[BLOCK_SIZE];

int last_log_block;
int segment_index;
int current_i_nodes;
int current_allocated_blocks;

void print_block(unsigned char buffer[]){
	for(int i = 1; i <= BLOCK_SIZE; i++){
		printf("%02x ", buffer[i - 1]);
		if(i % 8 == 0){
			printf("\n");
		}
	}
	printf("\n");
}

void wipe_segment(){
	memset(segment_buffer, 0, SEG_BLOCKS * BLOCK_SIZE);
	segment_index = 0;
}

// 0 bit means allocated, 1 bit means free
void mark_block_allocated(int index){
	free_vector[index / 8] = free_vector[index / 8] & 127 >> (index % 8);
}

// return 1 means allocated, return 0 means free
int check_if_block_allocated(int index){
	return free_vector[index / 8] == (free_vector[index / 8] & 127 >> (index % 8));
}

void create_superblock(){
	int magic = MAGIC;
	int num_blocks = NUM_BLOCKS;
	int num_i_nodes = NUM_I_NODES;
	memcpy(segment_buffer, &magic, sizeof(int));	// first 4 bytes
	memcpy(segment_buffer + sizeof(int) * 1, &num_blocks, sizeof(int)); // next 4 bytes
	memcpy(segment_buffer + sizeof(int) * 2, &num_i_nodes, sizeof(int)); // next 4
}

void check_superblock(){
	unsigned char buffer[BLOCK_SIZE];
	memset(buffer, 0, BLOCK_SIZE);
	read_block(buffer, 0);
	int magic, num_blocks, num_i_nodes;
	memcpy(&magic, buffer, sizeof(int));
	memcpy(&num_blocks, buffer + sizeof(int), sizeof(int));
	memcpy(&num_i_nodes, buffer + sizeof(int) * 2, sizeof(int));
	if(magic != MAGIC || num_blocks != NUM_BLOCKS || num_i_nodes != NUM_I_NODES){
		perror("Invalid Superblock!");
		exit(EXIT_FAILURE);
	}
}

void create_free_block_vector(){
	memset(free_vector, 255, BLOCK_SIZE);
	for(int i = 0; i < 8; i++){ // assignment says to allocate first 10 but I don't know why
		mark_block_allocated(i); // mark blocks 0 - 7 allocated
	}
	memcpy(segment_buffer + sizeof(char) * BLOCK_SIZE, &free_vector, sizeof(char) * BLOCK_SIZE);
	current_allocated_blocks = 8;
}

void load_free_block_vector(){
	memset(free_vector, 255, BLOCK_SIZE);
	read_block(free_vector, 1);
	print_block(free_vector);
	current_allocated_blocks = 0;
	for(int i = 0; i < NUM_BLOCKS; i++){
		if(check_if_block_allocated(i)){
			current_allocated_blocks++;
		}
	}
}

void create_i_node_map(){
	memset(i_node_map, 0, BLOCK_SIZE);
	unsigned short i_node_location = 7; // roots i-node is initially found at block 7
	memcpy(i_node_map, &i_node_location, sizeof(unsigned short));
	memcpy(segment_buffer + sizeof(char) * BLOCK_SIZE * 2, &i_node_map, sizeof(char) * BLOCK_SIZE);
	current_i_nodes = 1; // we initialize disk to start with one root i-node
}

void load_i_node_map(){
	memset(i_node_map, 0, BLOCK_SIZE);
	read_block(i_node_map, 2);
	// initialize current number of i-nodes in file system
	current_i_nodes = 0;
	for(int i = 0; i < NUM_I_NODES; i++){
		if((short)i_node_map[i * 2] != 0)
			current_i_nodes++;
	}
}

// NOTE: right now root i-node is in an intial section which will never be accessible again
// might want to shift it up or reduce seg size or something
void create_root_i_node(){
	// initially directory table for root is empty but we still allocate a data block for it
	unsigned char i_node[BLOCK_SIZE];
	memset(i_node, 0, BLOCK_SIZE);

	int file_size = BLOCK_SIZE;
	int file_type = DIR_FILE;
	unsigned short directory_table_location = 6;

	memcpy(i_node, &file_size, sizeof(int));
	memcpy(i_node + sizeof(int) * 1, &file_type, sizeof(int));
	memcpy(i_node + sizeof(int) * 2, &directory_table_location, sizeof(unsigned short));
	memcpy(segment_buffer + sizeof(char) * BLOCK_SIZE * 7, &i_node, sizeof(char) * BLOCK_SIZE);
}

void sync_to_disk(){
	for(int i = 0; i < SEG_BLOCKS; i++){
		write_block(segment_buffer + (i * BLOCK_SIZE), last_log_block + i);
	}
	write_block(free_vector, 1);
	write_block(i_node_map, 2);

	if(last_log_block + SEG_BLOCKS == NUM_BLOCKS){
		last_log_block = SEG_BLOCKS;
	}else{
		last_log_block += SEG_BLOCKS;
	}
	wipe_segment();
}

// 28 bytes
// 4 bytes -> int size of file
// 4 bytes -> flag for dir or file
// 2 bytes -> pointer to block number
// + 9 more pointers
void create_i_node(int type){
	// find free data block
	// find free i-node block
	// prepare dir table
	// write data block
	// allocate data block
	// write i-node
	// allocate i-node
}

// INTERFACING FUNCTIONS

void initLLFS(){
	wipe_disk();	// delete any existing disk
	attach_disk();	// initializes file_pointer
	last_log_block = 0;
	wipe_segment();
	create_superblock();
	create_free_block_vector();
	create_i_node_map();
	create_root_i_node();

	// sync the segment to disk
	sync_to_disk();
}

void attatchLLFS(){
	attach_disk();				// initializes file_pointer
	last_log_block = SEG_BLOCKS;
	wipe_segment();
	check_superblock();			// read super block and confirm correctness
	load_free_block_vector();	// read and initialize i_node_map
	load_i_node_map();			// read and initialize free block
	// do some form of disk check
}

// Return 0 means sucess, return -1 means error
int create_file(char* path, int type){
	// TODO:parse path to get filename, for now assume all files in root
	//		ensure path depth is no larger than 4
	if(current_i_nodes >= NUM_I_NODES){ // check for free i-node block
		return -1;
	}
	if(current_allocated_blocks >= NUM_BLOCKS - 1){ // check for free block for i-node and data block
		return -1;
	}
	// ??? allocate the data blocks
	// create i-node with file name, file type, etc.
	// allocate i-node
	// create new direcotry entry in parent dir table
}

// Return 0 means sucess, return -1 means error
int delete_file(char* path){
	// TODO:parse path to get filename, for now assume all files in root
	//		ensure path depth is no larger than 4
	// find i-node for given file using path
	// if it's a dir then ensure it is empty by looking up dir table
	// de-allocate blocks pointed to in i-node
	// remove file name from parent dir listing
	// deallocate i-node
	return -1;
}

// Return 0 means sucess, return -1 means error
int write_to_file(char* path, int offset, int num_bytes, char* buffer){
	return -1;
}

// Return 0 means sucess, return -1 means error
int read_from_file(char* path, int offset, int num_bytes, char* buffer){
	return -1;
}

int main(){
	initLLFS();
	// attatchLLFS();
	printf("last_log_block: %d, segment_index: %d, current_i_nodes: %d, current_allocated_blocks: %d\n", last_log_block, segment_index, current_i_nodes, current_allocated_blocks);

	printf("END OF MAIN!\n");
}

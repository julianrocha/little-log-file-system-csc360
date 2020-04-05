#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../disk/disk_controller.h"
#include "./File.h"

#define MAGIC 42
#define NUM_I_NODES 256 // ID of i-node must occupy 1 byte
#define I_NODE_SIZE 32
#define MAX_I_NODE_BLOCK_POINTERS 10
#define DIR_FILE 0
#define DATA_FILE 1
#define SEG_BLOCKS 8
#define MAX_FILE_NAME_CHARS 30
#define MAX_DIR_ENTRIES 16
#define MAX_TREE_DEPTH 4
#define MAX_PATH_LENGTH MAX_FILE_NAME_CHARS * MAX_TREE_DEPTH + MAX_TREE_DEPTH

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

void print_segment_block(int index){
	unsigned char block[BLOCK_SIZE];
	memcpy(block, segment_buffer + index * BLOCK_SIZE, sizeof(char) * BLOCK_SIZE);
	print_block(block);
}

void update_i_node_map(int i_node_index, short i_node_block_index){
	i_node_map[i_node_index * 2] = i_node_block_index;
}

short i_node_to_block(char i_node_index){
	return (short)i_node_map[i_node_index * 2];
}

int add_i_node_to_map(short i_node_block_index){
	int i_node_index;
	for(i_node_index = 0; i_node_index < NUM_I_NODES; i_node_index++){
		if(i_node_to_block(i_node_index) == 0){
			break;
		}
	}
	update_i_node_map(i_node_index, i_node_block_index);
	current_i_nodes++;
	return i_node_index;
}

void remove_from_i_node_map(int i_node_index){
	i_node_map[i_node_index * 2] = 0;
	current_i_nodes--;
}

// return 1 means allocated, return 0 means free
// Uses Complement and AND to check if bit is set
int check_if_block_allocated(int index){
	return free_vector[index / 8] == (free_vector[index / 8] & ~ (128 >> (index % 8)));
}

// 0 bit means allocated, 1 bit means free
// Uses XOR to flip bit
void mark_block_allocated(int index){
	if(!check_if_block_allocated(index)){
		free_vector[index / 8] = free_vector[index / 8] ^ (128 >> (index % 8));
		current_allocated_blocks++;
	}
}

// 0 bit means allocated, 1 bit means free
// Uses XOR to flip bit
void deallocate_block(int index){
	if(check_if_block_allocated(index)){
		free_vector[index / 8] = free_vector[index / 8] ^ (128 >> (index % 8));
		current_allocated_blocks--;
	}
}

void wipe_segment(){
	memset(segment_buffer, 0, SEG_BLOCKS * BLOCK_SIZE);
	segment_index = 0;
}

void sync_to_disk(){
	for(int i = 0; i < SEG_BLOCKS; i++){
		write_block_to_disk(segment_buffer + (i * BLOCK_SIZE), last_log_block + i);
	}
	write_block_to_disk(free_vector, 1);
	write_block_to_disk(i_node_map, 2);

	if(last_log_block + SEG_BLOCKS == NUM_BLOCKS){
		last_log_block = SEG_BLOCKS;
	}else{
		last_log_block += SEG_BLOCKS;
	}
	wipe_segment();
}

// Almost all writes are to the end of the segment
int write_block(unsigned char block[]){
	// check for room in segment, sync to disk if no room
	if(segment_index == SEG_BLOCKS){
		sync_to_disk();
	}
	// write block to segment and allocate block
	memcpy(segment_buffer + sizeof(char) * BLOCK_SIZE * segment_index, block, sizeof(char) * BLOCK_SIZE);
	int block_index = last_log_block + segment_index;
	mark_block_allocated(block_index);
	segment_index++;
	return block_index;
}

// If index in segment, read from memory, else read from disk
void read_block(unsigned char block[], int index){
	if(index >= last_log_block && index < last_log_block + segment_index){
		int memory_index = index - last_log_block;
		memcpy(block, segment_buffer + sizeof(char) * BLOCK_SIZE * memory_index, sizeof(char) * BLOCK_SIZE);
	} else{
		read_block_from_disk(block, index);
	}
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
	read_block_from_disk(buffer, 0);
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
	current_allocated_blocks = 0;
	for(int i = 0; i < 8; i++){ // assignment says to allocate first 10 but I don't know why
		mark_block_allocated(i); // mark blocks 0 - 7 allocated
	}
	memcpy(segment_buffer + sizeof(char) * BLOCK_SIZE, &free_vector, sizeof(char) * BLOCK_SIZE);
}

void load_free_block_vector(){
	read_block_from_disk(free_vector, 1);
	current_allocated_blocks = 0;
	for(int i = 0; i < NUM_BLOCKS; i++){
		if(check_if_block_allocated(i)){
			current_allocated_blocks++;
		}
	}
}

void create_i_node_map(){
	memset(i_node_map, 0, BLOCK_SIZE);
	memset(i_node_map, 7, sizeof(char)); // roots i-node is initially found at block 7
	memcpy(segment_buffer + sizeof(char) * BLOCK_SIZE * 2, &i_node_map, sizeof(char) * BLOCK_SIZE);
	current_i_nodes = 1; // we initialize disk to start with one root i-node
}

void load_i_node_map(){
	memset(i_node_map, 0, BLOCK_SIZE);
	read_block_from_disk(i_node_map, 2);
	// initialize current number of i-nodes in file system
	current_i_nodes = 0;
	for(int i = 0; i < NUM_I_NODES; i++){
		if((short)i_node_map[i * 2] != 0)
			current_i_nodes++;
	}
}

void create_i_node(unsigned char block[], int file_size, int file_type, unsigned short block_pointers[]){
	memset(block, 0, BLOCK_SIZE);
	memcpy(block, &file_size, sizeof(int));
	memcpy(block + sizeof(int) * 1, &file_type, sizeof(int));
	for(int i = 0; i < MAX_I_NODE_BLOCK_POINTERS; i++){
		memcpy(block + (sizeof(int) * 2) + (sizeof(unsigned char) * i), &block_pointers[i], sizeof(unsigned short));
	}
}
// NOTE: right now root i-node is in an intial section which will never be accessible again
// might want to shift it up or reduce seg size or something
void create_root_i_node(){
	// initially directory table for root is empty but we still allocate a data block for it
	unsigned char i_node[BLOCK_SIZE];
	unsigned short block_pointers[] = {6,0,0,0,0,0,0,0,0,0,0};
	create_i_node(i_node, BLOCK_SIZE, DIR_FILE, block_pointers);
	memcpy(segment_buffer + sizeof(char) * BLOCK_SIZE * 7, &i_node, sizeof(char) * BLOCK_SIZE);
}

// returns index of free spot in directory table, returns -1 if name taken or no room
int create_dir_index_for_file_name(unsigned char dir_table[], char* file_name){
	int length = strnlen(file_name, MAX_FILE_NAME_CHARS);
	if(file_name[length] != '\0'){
		printf("Cannot create file! File name is too long!\n");
		return -1;
	}
	int dir_index = -1;
	unsigned char i_node_index;
	int i;
	for(i = 0; i < MAX_DIR_ENTRIES; i++){
		memcpy(&i_node_index, dir_table + (i * (2 + MAX_FILE_NAME_CHARS)), sizeof(char));
		if(i_node_index != 0){
			if(!strncmp(file_name, dir_table + 1 + (i * (2 + MAX_FILE_NAME_CHARS)), MAX_FILE_NAME_CHARS)){
				printf("Cannot create file! File name already exists.\n");
				dir_index = -1;
				break;
			}
		}else if(dir_index == -1){
			// pick the first empty dir index
			dir_index = i;
		}
	}
	if(i == MAX_DIR_ENTRIES && dir_index == -1){
		printf("Cannot create file! Max number of files in this directory reached.\n");
	}
	return dir_index;
}

int create_empty_file_structure(int type){
	// write empty block to segment and allocate block
	unsigned char data_block[BLOCK_SIZE];
	memset(data_block, 0, BLOCK_SIZE);

	int empty_block_index = write_block(data_block);

	// write i-node with file name, file type, and block pointer to segment, allocate block, update i-node map
	unsigned char i_node[BLOCK_SIZE];
	unsigned short block_pointers[] = {empty_block_index,0,0,0,0,0,0,0,0,0,0};
	create_i_node(i_node, BLOCK_SIZE, type, block_pointers);
	int i_node_block_index = write_block(i_node);
	int i_node_index = add_i_node_to_map(i_node_block_index);
}

void write_file_to_dir_table(unsigned char dir_table[], char i_node_index, char* file_name, int dir_index){
	int num_file_name_chars = strnlen(file_name, MAX_FILE_NAME_CHARS) + 1;
	memcpy(dir_table + (dir_index * (2 + MAX_FILE_NAME_CHARS)), &i_node_index, sizeof(char));
	memcpy(dir_table + (dir_index * (2 + MAX_FILE_NAME_CHARS)) + 1, file_name, num_file_name_chars);
}

// return dir_index = -1 if couldn't find entry in dir_table for file_name
void i_node_for_file_name(unsigned char dir_table[], char* file_name, unsigned char i_node[], int* i_node_block_index, int* i_node_index, int* dir_index){
	*dir_index = -1;
	for(int i = 0; i < MAX_DIR_ENTRIES; i++){
		memcpy(i_node_index, dir_table + (i * (2 + MAX_FILE_NAME_CHARS)), sizeof(char));
		if(i_node_index != 0 && !strncmp(file_name, dir_table + 1 + (i * (2 + MAX_FILE_NAME_CHARS)), MAX_FILE_NAME_CHARS)){
			*dir_index = i;
			break;
		}
	}
	if(*dir_index == -1){
		return; // couldn't find entry in dir_table for file_name
	}
	*i_node_block_index = i_node_to_block(*i_node_index);
	read_block(i_node, *i_node_block_index);
}

// TODO: cannot handle paths that contain //, also strtok() is not multithreaded
void parent_structure_for_path(char* path, unsigned char file_name[], int* parent_i_node_index, int* parent_i_node_block_index, unsigned char parent_i_node[], int* parent_dir_table_block_index, unsigned char parent_dir_table[]){
	if(path[strnlen(path, MAX_PATH_LENGTH)] != '\0'){ // Error: path is too long
		*parent_i_node_index = -1;
		return;
	}
	if(path[0] != '/'){ // Error: path must begin with root
		*parent_i_node_index = -1;
		return;
	}
	
	// determine tree depth, must not be greater than 4
	int depth = 0;
	char tokenized_path[MAX_PATH_LENGTH];
	strncpy(tokenized_path, path, MAX_PATH_LENGTH);
	char* token = strtok(tokenized_path, "/");
	while(token != NULL){
		depth++;
		token = strtok(NULL, "/"); 
	}
	if(depth > MAX_TREE_DEPTH){ // tree is too deep
		*parent_i_node_index = -1;
		return;
	}

	*parent_i_node_index = 0; // start at root
	*parent_i_node_block_index = i_node_to_block(*parent_i_node_index);
	read_block(parent_i_node, *parent_i_node_block_index);

	memcpy(parent_dir_table_block_index, parent_i_node + sizeof(int) * 2, sizeof(unsigned short));
	read_block(parent_dir_table, *parent_dir_table_block_index);

	strncpy(tokenized_path, path, MAX_PATH_LENGTH);
	token = strtok(tokenized_path, "/");
	while(token != NULL && depth > 1){
		if(token[strnlen(token, MAX_FILE_NAME_CHARS)] != '\0'){ // directory name is too long
			*parent_i_node_index = -1;
			return;
		}
		int dir_index, type;
		i_node_for_file_name(parent_dir_table, token, parent_i_node, parent_i_node_block_index, parent_i_node_index, &dir_index);
		if(dir_index == -1){ // could not find this directory
			printf("Could not find this direcotry.\n");
			*parent_i_node_index = -1;
			return;
		}
		memcpy(&type, parent_i_node + sizeof(int), sizeof(int));
		if(type != DIR_FILE){ // a data file cannot be in the middle of the path
			printf("A data file cannot be in the middle of the path\n");
			*parent_i_node_index = -1;
			return;
		}

		memcpy(parent_dir_table_block_index, parent_i_node + sizeof(int) * 2, sizeof(unsigned short));
		read_block(parent_dir_table, *parent_dir_table_block_index);
		depth--;
		token = strtok(NULL, "/"); 
	}
	
	if(token[strnlen(token, MAX_FILE_NAME_CHARS)] != '\0'){ // file name is too long
		*parent_i_node_index = -1;
		return;
	}
	strncpy(file_name, token, MAX_FILE_NAME_CHARS);	// Return file_name
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
	if(current_i_nodes >= NUM_I_NODES){ // check for free i-node block
		printf("Cannot create file! Max number of files on disk reached.\n");
		return -1;
	}
	if(current_allocated_blocks >= NUM_BLOCKS - 1){
		// check for free block for i-node and data block, this operation should only net 2 new blocks
		printf("Cannot create file! No more room on disk.\n");
		return -1;
	}

	int parent_i_node_index, parent_i_node_block_index, parent_dir_table_block_index;
	unsigned char parent_i_node[BLOCK_SIZE];
	unsigned char parent_dir_table[BLOCK_SIZE];
	unsigned char file_name[MAX_FILE_NAME_CHARS + 1];
	parent_structure_for_path(path, file_name, &parent_i_node_index, &parent_i_node_block_index, parent_i_node, &parent_dir_table_block_index, parent_dir_table);
	// Error occured if parent_i_node_index is -1
	if(parent_i_node_block_index == -1){
		return -1;
	}

	int dir_index = create_dir_index_for_file_name(parent_dir_table, file_name);
	if(dir_index == -1){
		return -1;
	}
	// FINISHED REQUEST VALIDATION -> BEGIN CREATING FILE


	int i_node_index = create_empty_file_structure(type);

	// update parent dir table and write to segment, deallocate prev block, allocate new block
	write_file_to_dir_table(parent_dir_table, i_node_index, file_name, dir_index);
	deallocate_block(parent_dir_table_block_index);
	parent_dir_table_block_index = write_block(parent_dir_table);
	// update parent i-node to point to new dir table and write to segment, deallocate prev block, allocate new block, update i-node map
	unsigned short parent_block_pointers[] = {parent_dir_table_block_index,0,0,0,0,0,0,0,0,0,0};
	create_i_node(parent_i_node, BLOCK_SIZE, DIR_FILE, parent_block_pointers);
	deallocate_block(parent_i_node_block_index);
	parent_i_node_block_index = write_block(parent_i_node);
	update_i_node_map(parent_i_node_index, parent_i_node_block_index);
	return 0;
}

// Return 0 means sucess, return -1 means error
int delete_file(char* path){
	// 		return -1 if trying to delete root
	if(!strncmp(path, "/", MAX_FILE_NAME_CHARS)){
		printf("Cannot delete file! You may not delete the root directory.\n");
		return -1;
	}

	int parent_i_node_index, parent_i_node_block_index, parent_dir_table_block_index;
	unsigned char parent_i_node[BLOCK_SIZE];
	unsigned char parent_dir_table[BLOCK_SIZE];
	unsigned char file_name[MAX_FILE_NAME_CHARS + 1];
	parent_structure_for_path(path, file_name, &parent_i_node_index, &parent_i_node_block_index, parent_i_node, &parent_dir_table_block_index, parent_dir_table);
	// Error occured if parent_i_node_index is -1
	if(parent_i_node_block_index == -1){
		return -1;
	}

	// find i_node block, block index, i_node index, dir_index
	// 		return -1 if couldn't find i_node for file_name
	unsigned char i_node[BLOCK_SIZE];
	int i_node_block_index, i_node_index, dir_index;
	i_node_for_file_name(parent_dir_table, file_name, i_node, &i_node_block_index, &i_node_index, &dir_index);
	if(dir_index == -1){
		printf("Cannot delete file! Could not find file at that path.\n");
		return -1;
	}

	// if dir, find and return dir table block and ensure empty
	//		return -1 if dir is not empty
	int type, dir_table_block_index, index;
	memcpy(&type, i_node + sizeof(int), sizeof(int));
	memcpy(&dir_table_block_index, i_node + sizeof(int) * 2, sizeof(unsigned short));
	if(type == DIR_FILE){
		unsigned char dir_table[BLOCK_SIZE];
		read_block(dir_table, dir_table_block_index);
		for(int i = 0; i < MAX_DIR_ENTRIES; i++){
			memcpy(&index, dir_table + (i * (2 + MAX_FILE_NAME_CHARS)), sizeof(char));
			if(index != 0){
				printf("Cannot delete file! Directory must be empty before deleting.\n");
				return -1; // cannot delete dir as it is not empty
			}
		}
	}
	// FINISHED REQUEST VALIDATION -> BEGIN DELETING FILE

	// de_allocate all data blocks for i_node
	unsigned short block_pointer;
	for(int i = 0; i < MAX_I_NODE_BLOCK_POINTERS; i++){
		memcpy(&block_pointer, i_node + (sizeof(int) * 2) + (sizeof(unsigned char) * i), sizeof(unsigned short));
		if(block_pointer != 0){
			deallocate_block(block_pointer);
		}
	}

	// de_allocate i_node block and remove from i_node map
	deallocate_block(i_node_block_index);
	remove_from_i_node_map(i_node_index);

	// clear entry from parent dir table, de_allocate prev parent dir table, write and allocate new parent dir table
	memset(parent_dir_table + dir_index * (2 + MAX_FILE_NAME_CHARS), 0, MAX_FILE_NAME_CHARS + 2);
	deallocate_block(parent_dir_table_block_index);
	parent_dir_table_block_index = write_block(parent_dir_table);
	// update block pointer in parent i_node, de_allocate prev parent i_node, write and allocate new parent dir table, update_i_node map
	unsigned short parent_block_pointers[] = {parent_dir_table_block_index,0,0,0,0,0,0,0,0,0,0};
	create_i_node(parent_i_node, BLOCK_SIZE, DIR_FILE, parent_block_pointers);
	deallocate_block(parent_i_node_block_index);
	parent_i_node_block_index = write_block(parent_i_node);
	update_i_node_map(parent_i_node_index, parent_i_node_block_index);
	return 0;
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

	int status;
	status = create_file("/hello_world", DIR_FILE);
	printf("status: %d\n", status);
	status = create_file("/hello_world/cando", DATA_FILE);
	printf("status: %d\n", status);
	status = delete_file("/hello_world/cando");
	printf("status: %d\n", status);

	sync_to_disk();

	printf("i-node 0 is at block: %d, i-node 1 is at block: %d, i-node 2 is at block: %d\n",i_node_to_block(0), i_node_to_block(1), i_node_to_block(2));
	printf("last_log_block: %d, segment_index: %d, current_i_nodes: %d, current_allocated_blocks: %d\n", last_log_block, segment_index, current_i_nodes, current_allocated_blocks);

	printf("END OF MAIN!\n");
}

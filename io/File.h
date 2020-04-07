#ifndef FILE_H_
#define FILE_H_

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

// wipe disk if it exists, create fresh disk with root directory
void initLLFS();

// attatch to LLFS on an existing disk
void attatchLLFS();

// create new file/dir
// type 0 is dir, type 1 is data file
// Return 0 means sucess, return -1 means error
int create_file(char* path, int type);

// delete existing file/dir
// Return 0 means sucess, return -1 means error
int delete_file(char* path);

// overwrite file NOT dir
// Arguments
//	- path: absolute path to file, file must exist
//	- offset: number of bytes to offset from start of file, <=length(file)
//	- num_bytes: number of bytes from buffer to write to file, <= length(buffer)
//	- buffer: data to write to file
// Return 0 means sucess, return -1 means error
int write_to_file(char* path, unsigned int offset, unsigned int num_bytes, char buffer[]);

// read from file NOT dir
// Arguments
//	- path: absolute path to file, file must exist
//	- offset: number of bytes to offset from start of file, <length(file)
//	- num_bytes: number of bytes to read into buffer, <= length(buffer)
//	- buffer: location to read file data into
// Return 0 means sucess, return -1 means error
int read_from_file(char* path, unsigned int offset, unsigned int num_bytes, char buffer[]);

#endif

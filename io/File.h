#ifndef FILE_H_
#define FILE_H_

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
int write_to_file(char* path, int offset, int num_bytes, char* buffer);

// read from file NOT dir
// Arguments
//	- path: absolute path to file, file must exist
//	- offset: number of bytes to offset from start of file, <=length(file)
//	- num_bytes: number of bytes to read into buffer, <= length(buffer)
//	- buffer: location to read file data into
// Return 0 means sucess, return -1 means error
int read_from_file(char* path, int offset, int num_bytes, char* buffer);

#endif

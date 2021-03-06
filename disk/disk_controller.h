#ifndef DISK_CONTROLLER_H_
#define DISK_CONTROLLER_H_

#define BLOCK_SIZE 512
#define NUM_BLOCKS 4096
#define DISK_PATH "../disk/vdisk"

void write_block_to_disk(unsigned char block[], int index);

void read_block_from_disk(unsigned char block[], int index);

void attach_disk();

void wipe_disk();

#endif

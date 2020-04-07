#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../disk/disk_controller.h"
#include "../io/File.h"

void test01_read_from_file_root(){
	char buffer[4];
    if(read_from_file("/", 0, 4, buffer)){
        printf("test01 passed\n");
    } else{
        printf("test01 failed\n");
    }
}

void test02_read_from_file_does_not_exist(){
	char buffer[4];
    if(read_from_file("/whatfileisthisdoe", 0, 4, buffer)){
        printf("test02 passed\n");
    } else{
        printf("test02 failed\n");
    }
}

void test03_read_from_dir(){
	create_file("/lildirectory", DIR_FILE);
	char buffer[4];
    if(read_from_file("/lildirectory", 0, 4, buffer)){
        printf("test03 passed\n");
    } else{
        printf("test03 failed\n");
    }
}

void test04_read_from_file_out_of_range(){
	create_file("/lildirectory/lilfile", DATA_FILE);
	write_to_file("/lildirectory/lilfile", 0, 4, "test");
	char buffer[4];
    if(read_from_file("/lildirectory/lilfile", 1, 4, buffer)){
        printf("test04 passed\n");
    } else{
        printf("test04 failed\n");
    }
}

void test05_read_from_file_invalid_offset(){
	char buffer[4];
    if(read_from_file("/lildirectory/lilfile", 5, 4, buffer)){
        printf("test05 passed\n");
    } else{
        printf("test05 failed\n");
    }
}

void test06_read_from_file_empty(){
	create_file("/emptyfile", DATA_FILE);
	char buffer[4];
    if(read_from_file("/emptyfile", 0, 4, buffer)){
        printf("test06 passed\n");
    } else{
        printf("test06 failed\n");
    }
}

void test01_write_to_root(){
	char buffer[] = "test";
    if(write_to_file("/", 0, 4, buffer)){
            printf("test01 passed\n");
    } else{
            printf("test01 failed\n");
    }
}

void test02_write_to_file_does_not_exist(){
	char buffer[] = "test";
    if(write_to_file("/myfile", 0, 4, buffer)){
            printf("test02 passed\n");
    } else{
            printf("test02 failed\n");
    }
}

void test03_write_to_dir(){
	create_file("/mydir", DIR_FILE);
	char buffer[] = "test";
    if(write_to_file("/mydir", 0, 4, buffer)){
            printf("test03 passed\n");
    } else{
            printf("test03 failed\n");
    }
}

void test04_write_to_file_invalid_offset(){
	create_file("/mydir/myfile", DATA_FILE);
	char buffer[] = "test";
	write_to_file("/mydir/myfile", 0, 4, buffer);
    if(write_to_file("/mydir/myfile", -1, 4, buffer)){
		if(write_to_file("/mydir/myfile", 5, 4, buffer)){
			printf("test04 passed\n");
		} else{
			printf("test04 failed\n");
		}
    } else{
		printf("test04 failed\n");
    }
}

void test05_write_to_full_file(){
	char buffer[MAX_I_NODE_BLOCK_POINTERS * BLOCK_SIZE];
	write_to_file("/mydir/myfile", 0, MAX_I_NODE_BLOCK_POINTERS * BLOCK_SIZE, buffer);
	char buffer2[] = "test";
    if(write_to_file("/mydir/myfile", MAX_I_NODE_BLOCK_POINTERS * BLOCK_SIZE - 2, 4, buffer2)){
		printf("test05 passed\n");
    } else{
        printf("test05 failed\n");
    }
}

void test06_write_and_read_in_root(){
	create_file("/myfile", DATA_FILE);
	char buffer[] = "test";
    if(!write_to_file("/myfile", 0, 5, buffer)){
    	char buffer2[5];
    	read_from_file("/myfile", 0, 5, buffer2);
    	if(strcmp(buffer, buffer2)){
    		printf("test06 failed\n");
    	} else{
			printf("test06 passed\n");
    	}
    } else{
        printf("test06 failed\n");
    }
}

void test07_write_and_read_in_sub_dir(){
	create_file("/mydir", DIR_FILE);
	create_file("/mydir/myfile", DATA_FILE);
	char buffer[] = "test";
    if(!write_to_file("/mydir/myfile", 0, 5, buffer)){
    	char buffer2[5];
    	read_from_file("/mydir/myfile", 0, 5, buffer2);
    	if(strcmp(buffer, buffer2)){
    		printf("test07 failed\n");
    	} else{
			printf("test07 passed\n");
    	}
    } else{
        printf("test07 failed\n");
    }
}

void test09_write_to_file_start_of_block_and_some(){
	create_file("/blocky_file", DATA_FILE);
	char buffer[BLOCK_SIZE + 3];
	memset(buffer, 122, BLOCK_SIZE + 3);
    if(!write_to_file("/blocky_file", 0, BLOCK_SIZE + 3, buffer)){
    	char buffer2[BLOCK_SIZE + 3];
    	read_from_file("/blocky_file", 0, BLOCK_SIZE + 3, buffer2);
    	if(strncmp(buffer, buffer2, BLOCK_SIZE + 3)){
    		printf("test09 failed\n");
    	} else{
			printf("test09 passed\n");
    	}
    } else{
        printf("test09 failed\n");
    }
}

void test10_write_to_file_middle_of_block_and_some(){
	char buffer[BLOCK_SIZE + 3];
	memset(buffer, 120, BLOCK_SIZE + 3);
    if(!write_to_file("/blocky_file", 10, BLOCK_SIZE + 3, buffer)){
    	char buffer2[BLOCK_SIZE + 3];
    	read_from_file("/blocky_file", 10, BLOCK_SIZE + 3, buffer2);
    	if(strncmp(buffer, buffer2, BLOCK_SIZE + 3)){
    		printf("test10 failed\n");
    	} else{
			printf("test10 passed\n");
    	}
    } else{
        printf("test10 failed\n");
    }
}

void test11_write_to_file_start_of_block_to_middle(){
	char buffer[10];
	memset(buffer, 119, 10);
    if(!write_to_file("/blocky_file", 0, 10, buffer)){
    	char buffer2[10];
    	read_from_file("/blocky_file", 0, 10, buffer2);
    	if(strncmp(buffer, buffer2, 10)){
    		printf("test11 failed\n");
    	} else{
			printf("test11 passed\n");
    	}
    } else{
        printf("test11 failed\n");
    }
}

void test12_write_to_file_middle_of_block_to_middle(){
	char buffer[10];
	memset(buffer, 118, 10);
    if(!write_to_file("/blocky_file", 20, 10, buffer)){
    	char buffer2[10];
    	read_from_file("/blocky_file", 20, 10, buffer2);
    	if(strncmp(buffer, buffer2, 10)){
    		printf("test12 failed\n");
    	} else{
			printf("test12 passed\n");
    	}
    } else{
        printf("test12 failed\n");
    }
}


void test13_write_to_file_edit_middle_of_file(){
	create_file("/mybigfile", DATA_FILE);
	char buffer[BLOCK_SIZE * 3];
	memset(buffer, 115, BLOCK_SIZE * 3);
	write_to_file("/mybigfile", 0, BLOCK_SIZE * 3, buffer);
	char buffer2[BLOCK_SIZE + 2];
	memset(buffer2, 114, BLOCK_SIZE + 2);
    if(!write_to_file("/mybigfile", BLOCK_SIZE - 1, BLOCK_SIZE + 2, buffer2)){
    	char buffer3[BLOCK_SIZE + 2];
    	read_from_file("/mybigfile", BLOCK_SIZE - 1, BLOCK_SIZE + 2, buffer3);
    	if(strncmp(buffer2, buffer3, BLOCK_SIZE + 2)){
    		printf("test13 failed\n");
    	} else{
			printf("test13 passed\n");
    	}
    } else{
        printf("test13 failed\n");
    }
}
int main(){
	printf("BEGIN TEST SUITE\n");
	printf("BEGIN TESTS FOR READ_FROM_FILE()\n");
	// Situtations where read_from_file should error
	initLLFS(); // Start with clean disk
	test01_read_from_file_root();
	test02_read_from_file_does_not_exist();
	test03_read_from_dir();
	test04_read_from_file_out_of_range();
	test05_read_from_file_invalid_offset();
	test06_read_from_file_empty();


	printf("BEGIN TESTS FOR WRITE_TO_FILE()\n");
	// Situations where write_to_file should error
	initLLFS(); // Start with clean disk
	test01_write_to_root();
	test02_write_to_file_does_not_exist();
	test03_write_to_dir();
	test04_write_to_file_invalid_offset();
	test05_write_to_full_file();

	printf("BEGIN TESTS FOR WRITE_TO_FILE() AND READ_FROM_FILE()\n");
	// Situations where write_to_file should succeed
	// Testing both reading and writing files
	initLLFS(); // Start with clean disk
	test06_write_and_read_in_root();
	test07_write_and_read_in_sub_dir();
	test09_write_to_file_start_of_block_and_some();
	test10_write_to_file_middle_of_block_and_some();
	test11_write_to_file_start_of_block_to_middle();
	test12_write_to_file_middle_of_block_to_middle();
	test13_write_to_file_edit_middle_of_file();
	printf("END OF TEST SUITE\n");
}

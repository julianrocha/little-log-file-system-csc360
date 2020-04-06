#include <stdio.h>
#include <stdlib.h>
#include "../disk/disk_controller.h"
#include "../io/File.h"


void test01_create_file_path_too_long(){
	if(create_file("/hello_world/thisssss/path/iswaytooooooolonnng i mean come on how long could one path be i mean come on how long could one path be", DATA_FILE)){
		printf("test01 passed\n");
	} else{
		printf("test01 failed\n");
	}
}

void test02_create_file_without_root(){
	if(create_file("hello_world", DATA_FILE)){
		printf("test02 passed\n");
	} else{
		printf("test02 failed\n");
	}
}

void test03_create_file_path_too_deep(){
	if(create_file("/hello_world/this/path/is/toodeep", DATA_FILE)){
		printf("test03 passed\n");
	} else{
		printf("test03 failed\n");
	}
}

void test04_create_file_dir_name_too_long(){
	if(create_file("/hello_worldthisdirectoryyyyyyyy/istoolong", DATA_FILE)){
		printf("test04 passed\n");
	} else{
		printf("test04 failed\n");
	}
}

void test05_create_file_dir_in_path_does_not_exist(){
	if(create_file("/hello_world/wellthatdoesnotexist", DATA_FILE)){
		printf("test05 passed\n");
	} else{
		printf("test05 failed\n");
	}
}

void test06_create_file_file_name_too_long(){
	if(create_file("/toobadthathisfilenameistoolong.", DATA_FILE)){
		printf("test06 passed\n");
	} else{
		printf("test06 failed\n");
	}
}

void test07_create_file_data_file_in_middle_of_path(){
	int failure = create_file("/goodfile", DATA_FILE);
	if(!failure
		&& create_file("/goodfile/newfile", DATA_FILE)){
		printf("test07 passed\n");
	} else{
		printf("test07 failed\n");
	}
}

void test08_create_file_but_file_already_exists(){
	if(create_file("/goodfile", DATA_FILE)
		&& create_file("/goodfile", DIR_FILE)){
		printf("test08 passed\n");
	} else{
		printf("test08 failed\n");
	}
}

void test09_create_file_but_dir_has_too_many_files(){
	create_file("/my_directory", DIR_FILE);
	create_file("/my_directory/1", DATA_FILE);
	create_file("/my_directory/2", DATA_FILE);
	create_file("/my_directory/3", DATA_FILE);
	create_file("/my_directory/4", DATA_FILE);
	create_file("/my_directory/5", DATA_FILE);
	create_file("/my_directory/6", DATA_FILE);
	create_file("/my_directory/7", DATA_FILE);
	create_file("/my_directory/8", DATA_FILE);
	create_file("/my_directory/9", DATA_FILE);
	create_file("/my_directory/10", DATA_FILE);
	create_file("/my_directory/11", DATA_FILE);
	create_file("/my_directory/12", DATA_FILE);
	create_file("/my_directory/13", DATA_FILE);
	create_file("/my_directory/14", DATA_FILE);
	create_file("/my_directory/15", DATA_FILE);
	int failure = create_file("/my_directory/16", DATA_FILE);

	if(!failure && create_file("/my_directory/17", DATA_FILE)){
		printf("test09 passed\n");
	} else{
		printf("test09 failed\n");
	}
}

void test10_create_dir_in_sub_sub_sub_dir(){
	create_file("/a_directory", DIR_FILE);
	create_file("/a_directory/a_directory", DIR_FILE);
	create_file("/a_directory/a_directory/a_directory", DIR_FILE);
	if(create_file("/a_directory/a_directory/a_directory/a_directory", DIR_FILE)){
		printf("test10 passed\n");
	} else{
		printf("test10 failed\n");
	}
}

void test11_create_file_in_root(){
	if(!create_file("/data_file", DATA_FILE)){
		printf("test11 passed\n");
	} else{
		printf("test11 failed\n");
	}
}

void test12_create_dir_in_root(){
	if(!create_file("/a_directory", DIR_FILE)){
		printf("test12 passed\n");
	} else{
		printf("test12 failed\n");
	}
}

void test13_create_file_in_sub_dir(){
	if(!create_file("/a_directory/data_file", DATA_FILE)){
		printf("test13 passed\n");
	} else{
		printf("test13 failed\n");
	}
}

void test14_create_dir_in_sub_dir(){
	if(!create_file("/a_directory/a_directory", DIR_FILE)){
		printf("test14 passed\n");
	} else{
		printf("test14 failed\n");
	}
}

void test15_create_file_in_sub_sub_dir(){
	if(!create_file("/a_directory/a_directory/data_file", DATA_FILE)){
		printf("test15 passed\n");
	} else{
		printf("test15 failed\n");
	}
}

void test16_create_dir_in_sub_sub_dir(){
	if(!create_file("/a_directory/a_directory/a_directory", DIR_FILE)){
		printf("test16 passed\n");
	} else{
		printf("test16 failed\n");
	}
}

void test17_create_file_in_sub_sub_sub_dir(){
	if(!create_file("/a_directory/a_directory/a_directory/data_file", DATA_FILE)){
		printf("test17 passed\n");
	} else{
		printf("test17 failed\n");
	}
}

void test01_delete_root(){
	if(delete_file("/")){
		printf("test01 passed\n");
	} else{
		printf("test01 failed\n");
	}
}

void test02_delete_nonexistant_file_in_root(){
	if(delete_file("/whatisdis")){
		printf("test02 passed\n");
	} else{
		printf("test02 failed\n");
	}
}

void test03_delete_nonexistant_file_in_sub_dir(){
	if(delete_file("/my_directory/whatisdis")){
		printf("test03 passed\n");
	} else{
		printf("test03 failed\n");
	}
}

void test04_delete_nonempty_dir(){
	if(delete_file("/my_directory")){
		printf("test04 passed\n");
	} else{
		printf("test04 failed\n");
	}
}

void test05_delete_file_in_root(){
	if(!delete_file("/data_file")){
		printf("test05 passed\n");
	} else{
		printf("test05 failed\n");
	}
}

void test06_delete_file_in_sub_sub_sub_dir(){
	if(!delete_file("/a_directory/a_directory/a_directory/data_file")){
		printf("test06 passed\n");
	} else{
		printf("test06 failed\n");
	}
}

void test07_delete_dir_in_sub_sub_dir(){
	if(!delete_file("/a_directory/a_directory/a_directory")){
		printf("test07 passed\n");
	} else{
		printf("test07 failed\n");
	}
}

int main(){
	printf("BEGIN TEST SUITE\n");
	printf("BEGIN TESTS FOR CREATE_FILE()\n");
	
	// Situations where create_file should error
	initLLFS(); // Start with clean disk
	test01_create_file_path_too_long();
	test02_create_file_without_root();
	test03_create_file_path_too_deep();
	test04_create_file_dir_name_too_long();
	test05_create_file_dir_in_path_does_not_exist();
	test06_create_file_file_name_too_long();
	test07_create_file_data_file_in_middle_of_path();
	test08_create_file_but_file_already_exists();
	test09_create_file_but_dir_has_too_many_files();
	test10_create_dir_in_sub_sub_sub_dir();
	// Situations where create_file should succeed
	initLLFS(); // Start with clean disk
	test11_create_file_in_root();
	test12_create_dir_in_root();
	test13_create_file_in_sub_dir();
	test14_create_dir_in_sub_dir();
	test15_create_file_in_sub_sub_dir();
	test16_create_dir_in_sub_sub_dir();
	test17_create_file_in_sub_sub_sub_dir();

	printf("END OF TESTS FOR CREATE_FILE()\n");
	printf("BEGIN TESTS FOR DELETE_FILE()\n");
	
	// Situations where delete_file should error
	test01_delete_root();
	test02_delete_nonexistant_file_in_root();
	test03_delete_nonexistant_file_in_sub_dir();
	test04_delete_nonempty_dir();
	// Situations where delete_file should succeed 
	test05_delete_file_in_root();
	test06_delete_file_in_sub_sub_sub_dir();
	test07_delete_dir_in_sub_sub_dir();

	printf("END OF TESTS FOR DELETE_FILE()\n");

	printf("END OF TEST SUITE!\n");
}
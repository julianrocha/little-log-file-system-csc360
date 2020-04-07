CC = gcc
CFLAGS = -Wall

FILES = ../disk/disk_controller.c ../io/File.c

build:
	$(CC) -o test_create_and_delete.o test_create_and_delete.c $(FILES)
	$(CC) -o test_read_and_write.o test_read_and_write.c $(FILES)
	$(CC) -o test_disk_driver.o test_disk_driver.c $(FILES)
clean:
	rm -f *.o core

rebuild: clean build

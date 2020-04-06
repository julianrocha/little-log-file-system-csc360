CC = gcc
CFLAGS = -Wall

FILES = test_create_and_delete.c ../disk/disk_controller.c ../io/File.c
OUT_EXE = test_create_and_delete.o

build: $(FILES)
	$(CC) -o $(OUT_EXE) $(FILES)

clean:
	rm -f *.o core

rebuild: clean build

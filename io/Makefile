CC = gcc
CFLAGS = -Wall

FILES = File.c ../disk/disk_controller.c
OUT_EXE = File.o

build: $(FILES)
	$(CC) -o $(OUT_EXE) $(FILES)

clean:
	rm -f *.o core

rebuild: clean build
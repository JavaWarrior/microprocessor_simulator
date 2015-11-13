CC=gcc

all: main.c
	$(CC) main.c -o micro

clean:
	rm micro

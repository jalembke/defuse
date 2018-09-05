all: fusexmp

fusexmp: fusexmp.c
	gcc -Wall fusexmp.c `pkg-config fuse --cflags --libs` -o fusexmp

clean:
	rm -f fusexmp

all: fusexmp fusexmp_fh

fusexmp: fusexmp.c
	gcc -Wall fusexmp.c `pkg-config fuse --cflags --libs` -o fusexmp

fusexmp_fh: fusexmp_fh.c
	gcc -Wall fusexmp_fh.c `pkg-config fuse --cflags --libs` -lulockmgr -o fusexmp_fh

clean:
	rm -f fusexmp fusexmp_fh

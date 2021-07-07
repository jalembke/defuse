CFLAGS=-DHAVE_UTIMENSAT -DHAVE_POSIX_FALLOCATE -DHAVE_SETXATTR

all: fusexmp fusexmp_fh

fusexmp: fusexmp.c
	gcc -Wall fusexmp.c ${CFLAGS} `pkg-config fuse --cflags --libs` -o fusexmp

fusexmp_fh: fusexmp_fh.c
	gcc -Wall fusexmp_fh.c ${CFLAGS} `pkg-config fuse --cflags --libs` -lulockmgr -o fusexmp_fh

avfsd: avfsd.c
	gcc -Wall avfsd.c /usr/local/lib/libavfs.a `pkg-config fuse --cflags --libs` `avfs-config --cflags` -o avfds

clean:
	rm -f fusexmp fusexmp_fh

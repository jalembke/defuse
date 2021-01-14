#CC=musl-gcc
CC=gcc

ROOT=..

CFLAGS=-c -g -fPIC -I/usr/include/glib-2.0 -I/usr/lib/x86_64-linux-gnu/glib-2.0/include
LFLAGS=-shared

all: libusfs_wrap.so libsshfs_wrap.so

libusfs_wrap.so: usfs_wrap.o
	$(CC) -o $@ usfs_wrap.o $(LFLAGS)

libavfs_wrap.so: avfs_wrap.o
	$(CC) -o $@ avfs_wrap.o /usr/local/lib/libavfs.a $(LFLAGS)

libsshfs_wrap.so: sshfs_wrap.o cache.o
	$(CC) -o $@ ssh_wrap.o cache.o `pkg-config fuse --libs` $(LFLAGS)

cleanobjs:
	rm -f usfs_wrap.o avfs_wrap.o

clean:
	rm -f usfs_wrap.o avfs_wrap.o sshfs_wrap.o cache.o
	rm -f libusfs_wrap.so libavfs_wrap.so libsshfs_wrap.so

usfs_wrap.o: usfs_wrap.c
	$(CC) $(CFLAGS) -o $@ $<

avfs_wrap.o: avfs_wrap.c
	$(CC) $(CFLAGS) `avfs-config --cflags` -o $@ $<

sshfs_wrap.o: sshfs/sshfs_wrap.c
	$(CC) $(CFLAGS) `pkg-config fuse --cflags` -o $@ $<

cache.o: sshfs/cache.c
	$(CC) $(CFLAGS) `pkg-config fuse --cflags` -o $@ $<

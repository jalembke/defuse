CC=musl-gcc

ROOT=..

CFLAGS=-c -g -fPIC
LFLAGS=-shared

all: libusfs_wrap.so libavfs_wrap.so

libusfs_wrap.so: usfs_wrap.o
	$(CC) -o $@ usfs_wrap.o $(LFLAGS)

libavfs_wrap.so: avfs_wrap.o
	$(CC) -o $@ avfs_wrap.o /usr/local/lib/libavfs.a $(LFLAGS)

cleanobjs:
	rm -f usfs_wrap.o avfs_wrap.o

clean:
	rm -f usfs_wrap.o avfs_wrap.o
	rm -f libusfs_wrap.so libavfs_wrap.so

usfs_wrap.o: usfs_wrap.c
	$(CC) $(CFLAGS) -o $@ $<

avfs_wrap.o: avfs_wrap.c
	$(CC) $(CFLAGS) `avfs-config --cflags` -o $@ $<

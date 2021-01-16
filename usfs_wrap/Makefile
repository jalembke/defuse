#CC=musl-gcc
CC=gcc

ROOT=..
CRUISE_ROOT=../../cruise

CFLAGS=-c -g -fPIC
LFLAGS=-shared

AVFS_CFLAGS = `avfs-config --cflags`

SSHFS_CFLAGS = $(CFLAGS) -I/usr/include/glib-2.0 -I/usr/lib/x86_64-linux-gnu/glib-2.0/include `pkg-config fuse --cflags`
SSHFS_LFLAGS = `pkg-config fuse --libs` $(LFLAGS) 

CRUISE_CFLAGS = $(CFLAGS) -I$(CRUISE_ROOT) -I$(CRUISE_ROOT)/src
CRUISE_LFLAGS = $(LFLAGS) -L$(CRUISE_ROOT)/src -lcruise-posix -pthread

all: libusfs_wrap.so libsshfs_wrap.so libcruise_wrap.so

libusfs_wrap.so: usfs_wrap.o
	$(CC) -o $@ usfs_wrap.o $(LFLAGS)

libavfs_wrap.so: avfs_wrap.o
	$(CC) -o $@ avfs_wrap.o /usr/local/lib/libavfs.a $(LFLAGS)

libsshfs_wrap.so: sshfs_wrap.o cache.o
	$(CC) -o $@ sshfs_wrap.o cache.o $(SSHFS_LFLAGS)

libcruise_wrap.so: cruise_wrap.o
	$(CC) -o $@ cruise_wrap.o $(CRUISE_LFLAGS)

cleanobjs:
	rm -f usfs_wrap.o avfs_wrap.o sshfs_wrap.o cache.o cruise_wrap.o

clean:
	rm -f usfs_wrap.o avfs_wrap.o sshfs_wrap.o cache.o cruise_wrap.o
	rm -f libusfs_wrap.so libavfs_wrap.so libsshfs_wrap.so libcruise_wrap.so

usfs_wrap.o: usfs_wrap.c
	$(CC) $(CFLAGS) -o $@ $<

avfs_wrap.o: avfs_wrap.c
	$(CC) $(CFLAGS) `avfs-config --cflags` -o $@ $<

sshfs_wrap.o: sshfs/sshfs_wrap.c
	$(CC) $(SSHFS_CFLAGS) -o $@ $<

cache.o: sshfs/cache.c
	$(CC) $(SSHFS_CFLAGS) -o $@ $<

cruise_wrap.o: cruise_wrap.c
	$(CC) $(CRUISE_CFLAGS) -o $@ $<

#
# Makefile for the Proxy filesystem.
#
obj-m += proxyfs.o
proxyfs-objs := inode.o file.o

CFLAGS_inode.o := -DDEBUG -DDEBUG_PROXYFS

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

#
obj-m += bopfs.o
bopfs-objs := inode.o

#LOCAL_CFLAGS := -DDEBUG -DDEBUG_BOPFS
LOCAL_CFLAGS :=

CFLAGS_inode.o := $(LOCAL_CFLAGS)

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

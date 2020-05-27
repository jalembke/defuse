#
obj-m += defuse.o
defuse-objs := inode.o file.o

#LOCAL_CFLAGS := -DDEBUG -DDEBUG_DEFUSE
LOCAL_CFLAGS :=

CFLAGS_inode.o := $(LOCAL_CFLAGS)
CFLAGS_file.o := $(LOCAL_CFLAGS)

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

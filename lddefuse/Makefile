CC=gcc
CXX=g++
AR=ar

ROOT=..
LMDBLIB=$(HOME)/lib

DEBUGFLAGS= -DDEBUG_LDDEFUSE

CFLAGS=-c -g -fPIC
CXXFLAGS=-c -g -fPIC -std=c++0x
LFLAGS=-ldl -lrt -shared

DEFUSE_MOUNT=/tmp/usfsal
DEFUSE_BACKEND=/
DEFUSE_MNT_BACKEND=/mnt/test
DEFUSE_EXT4_BACKEND=/mnt/ext4
DEFUSE_JFS_BACKEND=/mnt/jfs
DEFUSE_FAT_BACKEND=/mnt/fat
DEFUSE_TMPFS_BACKEND=/mnt/tmpfs
#DEFUSE_AVFS_BACKEND=/mnt/test/test.tar.gz\#
DEFUSE_AVFS_BACKEND=/

#DEFUSECONFIGFLAGS= -DDEFUSE_DO_REAL_OPEN -DDEFUSE_MOUNT=\"${DEFUSE_MOUNT}\" -DDEFUSE_BACKEND=\"${DEFUSE_BACKEND}\"
#DEFUSECONFIGFLAGS= -DDEFUSE_DO_REAL_OPEN -DDEFUSE_MOUNT=\"${DEFUSE_MOUNT}\"
#DEFUSECONFIGFLAGS= -DDEFUSE_DO_NULL_OPEN -DDEFUSE_MOUNT=\"${DEFUSE_MOUNT}\"
DEFUSECONFIGFLAGS= -DDEFUSE_DO_NULL_OPEN -DDEFUSE_MOUNT=\"${DEFUSE_MOUNT}\"

OBJS = $(patsubst %.cpp,%.o,$(wildcard *.cpp)) $(patsubst %.c,%.o,$(wildcard *.c))

all: CFLAGS += -DDEFUSE_BACKEND=\"${DEFUSE_BACKEND}\"
all: CXXFLAGS += -DDEFUSE_BACKEND=\"${DEFUSE_BACKEND}\"
all: cleanobjs liblddefuse.so

mnt: CFLAGS += -DDEFUSE_BACKEND=\"${DEFUSE_MNT_BACKEND}\"
mnt: CXXFLAGS += -DDEFUSE_BACKEND=\"${DEFUSE_MNT_BACKEND}\"
mnt: cleanobjs liblddefuse.mnt.so

ext4: CFLAGS += -DDEFUSE_BACKEND=\"${DEFUSE_EXT4_BACKEND}\"
ext4: CXXFLAGS += -DDEFUSE_BACKEND=\"${DEFUSE_EXT4_BACKEND}\"
ext4: cleanobjs liblddefuse.mnt.so

jfs: CFLAGS += -DDEFUSE_BACKEND=\"${DEFUSE_JFS_BACKEND}\"
jfs: CXXFLAGS += -DDEFUSE_BACKEND=\"${DEFUSE_JFS_BACKEND}\"
jfs: cleanobjs liblddefuse.jfs.so

fat: CFLAGS += -DDEFUSE_BACKEND=\"${DEFUSE_FAT_BACKEND}\"
fat: CXXFLAGS += -DDEFUSE_BACKEND=\"${DEFUSE_FAT_BACKEND}\"
fat: cleanobjs liblddefuse.fat.so

tmpfs: CFLAGS += -DDEFUSE_BACKEND=\"${DEFUSE_TMPFS_BACKEND}\"
tmpfs: CXXFLAGS += -DDEFUSE_BACKEND=\"${DEFUSE_TMPFS_BACKEND}\"
tmpfs: cleanobjs liblddefuse.tmpfs.so

avfs: CFLAGS += -DDEFUSE_BACKEND=\"${DEFUSE_AVFS_BACKEND}\"
avfs: CXXFLAGS += -DDEFUSE_BACKEND=\"${DEFUSE_AVFS_BACKEND}\"
avfs: LFLAGS += ${ROOT}/avfs-1.0.6/lib/.libs/libavfs_static.a
avfs: cleanobjs liblddefuse.avfs.so

debug: CFLAGS += $(DEBUGFLAGS)
debug: CXXFLAGS += $(DEBUGFLAGS)
debug: cleanobjs $(word 2, ${MAKECMDGOALS}) 

cleanobjs:
	rm -f $(OBJS)

clean:
	rm -f $(OBJS)
	rm -f liblddefuse.*.so
	rm -f liblddefuse.so

liblddefuse.so: $(OBJS)
	$(CXX) -o $@ $(OBJS) $(LFLAGS)

liblddefuse.%.so: $(OBJS)
	$(CXX) -o $@ $(OBJS) $(LFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) $(DEFUSECONFIGFLAGS) -o $@ $<

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(DEFUSECONFIGFLAGS) -o $@ $<

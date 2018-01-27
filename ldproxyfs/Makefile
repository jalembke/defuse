CC=gcc
CXX=g++
AR=ar

ROOT=..
LMDBLIB=$(HOME)/lib

DEBUGFLAGS= -DDEBUG_LDPROXYFS

CFLAGS=-c -g -fPIC
CXXFLAGS=-c -g -fPIC -std=c++0x
LFLAGS=-ldl -shared

PROXYFS_MOUNT=/tmp/tmpdir
PROXYFS_MNT_BACKEND=/mnt
PROXYFS_EXT4_BACKEND=/mnt/ext4
PROXYFS_JFS_BACKEND=/mnt/jfs
PROXYFS_FAT_BACKEND=/mnt/fat
PROXYFS_TMPFS_BACKEND=/mnt/tmpfs
PROXYFS_AVFS_BACKEND=/mnt/tmpfs/test.tar.gz\#

#PROXYFSCONFIGFLAGS= -DPROXYFS_DO_REAL_OPEN -DPROXYFS_MOUNT=\"${PROXYFS_MOUNT}\" -DPROXYFS_BACKEND=\"${PROXYFS_BACKEND}\"
#PROXYFSCONFIGFLAGS= -DPROXYFS_DO_REAL_OPEN -DPROXYFS_MOUNT=\"${PROXYFS_MOUNT}\"
#PROXYFSCONFIGFLAGS= -DPROXYFS_DO_NULL_OPEN -DPROXYFS_MOUNT=\"${PROXYFS_MOUNT}\"
PROXYFSCONFIGFLAGS= -DPROXYFS_DO_NO_OPEN -DPROXYFS_MOUNT=\"${PROXYFS_MOUNT}\"

OBJS = $(patsubst %.cpp,%.o,$(wildcard *.cpp)) $(patsubst %.c,%.o,$(wildcard *.c))

all:
	@echo "No all target, use: mnt, ext4, jfs, fat, tmpfs, or avfs"

mnt: CFLAGS += -DPROXYFS_BACKEND=\"${PROXYFS_MNT_BACKEND}\"
mnt: CXXFLAGS += -DPROXYFS_BACKEND=\"${PROXYFS_MNT_BACKEND}\"
mnt: cleanobjs libldproxyfs.mnt.so

ext4: CFLAGS += -DPROXYFS_BACKEND=\"${PROXYFS_EXT4_BACKEND}\"
ext4: CXXFLAGS += -DPROXYFS_BACKEND=\"${PROXYFS_EXT4_BACKEND}\"
ext4: cleanobjs libldproxyfs.ext4.so

jfs: CFLAGS += -DPROXYFS_BACKEND=\"${PROXYFS_JFS_BACKEND}\"
jfs: CXXFLAGS += -DPROXYFS_BACKEND=\"${PROXYFS_JFS_BACKEND}\"
jfs: cleanobjs libldproxyfs.jfs.so

fat: CFLAGS += -DPROXYFS_BACKEND=\"${PROXYFS_FAT_BACKEND}\"
fat: CXXFLAGS += -DPROXYFS_BACKEND=\"${PROXYFS_FAT_BACKEND}\"
fat: cleanobjs libldproxyfs.fat.so

tmpfs: CFLAGS += -DPROXYFS_BACKEND=\"${PROXYFS_TMPFS_BACKEND}\"
tmpfs: CXXFLAGS += -DPROXYFS_BACKEND=\"${PROXYFS_TMPFS_BACKEND}\"
tmpfs: cleanobjs libldproxyfs.tmpfs.so

avfs: CFLAGS += -DPROXYFS_BACKEND=\"${PROXYFS_AVFS_BACKEND}\"
avfs: CXXFLAGS += -DPROXYFS_BACKEND=\"${PROXYFS_AVFS_BACKEND}\"
avfs: LFLAGS += ../../avfs-1.0.5/lib/.libs/libavfs_static.a
avfs: cleanobjs libldproxyfs.avfs.so

debug: CFLAGS += $(DEBUGFLAGS)
debug: CXXFLAGS += $(DEBUGFLAGS)
debug: cleanobjs $(word 2, ${MAKECMDGOALS}) 

cleanobjs:
	rm -f $(OBJS)

clean:
	rm -f $(OBJS)
	rm -f libldproxyfs.*.so

libldproxyfs.%.so: $(OBJS)
	$(CXX) -o $@ $(OBJS) $(LFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) $(PROXYFSCONFIGFLAGS) -o $@ $<

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(PROXYFSCONFIGFLAGS) -o $@ $<

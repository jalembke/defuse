CC=gcc
CXX=g++
AR=ar

ROOT=..
LMDBLIB=$(HOME)/lib

DEBUGFLAGS= -DDEBUG_LDPROXYFS

CFLAGS=-c -g -fPIC
CXXFLAGS=-c -g -fPIC
LFLAGS=-ldl -shared

PROXYFS_MOUNT=/tmp/tmpdir
PROXYFS_BACKEND=/mnt/tmpbackend
PROXYFS_EXT4_BACKEND=/mnt/ext4
PROXYFS_JFS_BACKEND=/mnt/jfs
PROXYFS_FAT_BACKEND=/mnt/fat
PROXYFS_TMPFS_BACKEND=/mnt/tmpfs

#PROXYFSCONFIGFLAGS= -DPROXYFS_DO_REAL_OPEN -DPROXYFS_MOUNT=\"${PROXYFS_MOUNT}\" -DPROXYFS_BACKEND=\"${PROXYFS_BACKEND}\"
PROXYFSCONFIGFLAGS= -DPROXYFS_DO_REAL_OPEN -DPROXYFS_MOUNT=\"${PROXYFS_MOUNT}\"

OBJS = $(patsubst %.cpp,%.o,$(wildcard *.cpp)) $(patsubst %.c,%.o,$(wildcard *.c))

all: libldproxyfs.so

ext4: CFLAGS += -DPROXYFS_BACKEND=\"${PROXYFS_EXT4_BACKEND}\"
ext4: CXXFLAGS += -DPROXYFS_BACKEND=\"${PROXYFS_EXT4_BACKEND}\"
ext4: clean libldproxyfs.ext4.so

jfs: CFLAGS += -DPROXYFS_BACKEND=\"${PROXYFS_JFS_BACKEND}\"
jfs: CXXFLAGS += -DPROXYFS_BACKEND=\"${PROXYFS_JFS_BACKEND}\"
jfs: clean libldproxyfs.jfs.so

fat: CFLAGS += -DPROXYFS_BACKEND=\"${PROXYFS_FAT_BACKEND}\"
fat: CXXFLAGS += -DPROXYFS_BACKEND=\"${PROXYFS_FAT_BACKEND}\"
fat: clean libldproxyfs.fat.so

tmpfs: CFLAGS += -DPROXYFS_BACKEND=\"${PROXYFS_TMPFS_BACKEND}\"
tmpfs: CXXFLAGS += -DPROXYFS_BACKEND=\"${PROXYFS_TMPFS_BACKEND}\"
tmpfs: clean libldproxyfs.tmpfs.so

debug: CFLAGS += $(DEBUGFLAGS)
debug: CXXFLAGS += $(DEBUGFLAGS)
debug: clean all

clean:
	rm -f $(OBJS)
	rm -f libldproxyfs.so

libldproxyfs.so: $(OBJS)
	$(CXX) -o $@ $(OBJS) $(LFLAGS)

libldproxyfs.%.so: $(OBJS)
	$(CXX) -o $@ $(OBJS) $(LFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) $(PROXYFSCONFIGFLAGS) -o $@ $<

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(PROXYFSCONFIGFLAGS) -o $@ $<

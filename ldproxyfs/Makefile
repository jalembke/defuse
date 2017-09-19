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
PROXYFS_BACKEND=/tmp/tmpb

PROXYFSCONFIGFLAGS= -DPROXYFS_DO_REAL_OPEN -DPROXYFS_MOUNT=\"${PROXYFS_MOUNT}\" -DPROXYFS_BACKEND=\"${PROXYFS_BACKEND}\"

OBJS = $(patsubst %.cpp,%.o,$(wildcard *.cpp)) $(patsubst %.c,%.o,$(wildcard *.c))

all: libldproxyfs.so

debug: CFLAGS += $(DEBUGFLAGS)
debug: CXXFLAGS += $(DEBUGFLAGS)
debug: clean all

clean:
	rm -f $(OBJS)
	rm -f libldhykefs.so

libldproxyfs.so: $(OBJS)
	$(CXX) -o $@ $(OBJS) $(LFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) $(PROXYFSCONFIGFLAGS) -o $@ $<

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(PROXYFSCONFIGFLAGS) -o $@ $<

CC=gcc
CXX=g++
AR=ar

ROOT=..
HYKEFSROOT=$(ROOT)/src
LMDBLIB=$(HOME)/lib

DEBUGFLAGS= -DDEBUG_LDHYKEFS

CFLAGS=-c -g -fPIC -I$(HYKEFSROOT)
CXXFLAGS=-c -g -fPIC -I$(HYKEFSROOT)
LFLAGS=-L$(HYKEFSROOT) -L$(LMDBLIB) -lhykefs -llmdb -lrt -ldl -shared
#LFLAGS=--shared -static -nostdlib -lstdc++ -l:/home/lembkej/glibc-2.12.1_build/libc_pic.a -l:/home/lembkej/hykefs/src/libhykefs.a -l:/home/lembkej/lib/liblmdb.a -lpthread -ldl -l:/opt/sensu/embedded/lib/libcrypto.a

HYKEFS_MOUNT=/tmp/tmpdir
HYKEFS_BACKEND_STRING=lmdb:${RCAC_SCRATCH}/hykefs
#HYKEFS_BACKEND_STRING=lmdb:/tmp/lembkej/hykefs
HYKEFS_MAX_FILE_SIZE=262144
#HYKEFS_MAX_FILE_SIZE=2073741824
HYKEFS_MAX_HANDLES=750
HYKEFS_OPEN=NULL

HYKEFSCONFIGFLAGS= -DHYKEFS_DO_${HYKEFS_OPEN}_OPEN -DHYKEFS_MOUNT=\"${HYKEFS_MOUNT}\" -DHYKEFS_BACKEND_STRING=\"${HYKEFS_BACKEND_STRING}\" -DHYKEFS_MAX_FILE_SIZE=${HYKEFS_MAX_FILE_SIZE} -DHYKEFS_MAX_HANDLES=${HYKEFS_MAX_HANDLES}

OBJS = $(patsubst %.cpp,%.o,$(wildcard *.cpp)) $(patsubst %.c,%.o,$(wildcard *.c))

all: libldhykefs.so

debug: CFLAGS += $(DEBUGFLAGS)
debug: CXXFLAGS += $(DEBUGFLAGS)
debug: clean all

clean:
	rm -f $(OBJS)
	rm -f libldhykefs.so

libldhykefs.so: $(OBJS)
	$(CXX) -o $@ $(OBJS) $(LFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) $(HYKEFSCONFIGFLAGS) -o $@ $<

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(HYKEFSCONFIGFLAGS) -o $@ $<

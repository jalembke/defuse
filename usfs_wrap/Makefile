CC=musl-gcc
CXX=g++
AR=ar

ROOT=..

CFLAGS=-c -g -fPIC
CXXFLAGS=-c -g -fPIC -std=c++0x
LFLAGS=-shared

OBJS = $(patsubst %.cpp,%.o,$(wildcard *.cpp)) $(patsubst %.c,%.o,$(wildcard *.c))

all: libusfs_wrap.so

libusfs_wrap.so: $(OBJS)
	$(CC) -o $@ $(OBJS) $(LFLAGS)

cleanobjs:
	rm -f $(OBJS)

clean:
	rm -f $(OBJS)
	rm -f libusfs_wrap.so

%o : %.c
	$(CC) $(CFLAGS) -o $@ $<

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(DEFUSECONFIGFLAGS) -o $@ $<

CC=gcc
CXX=g++
AR=ar

ROOT=..
LIBDEFUSE=$(ROOT)/libdefuse

CFLAGS=-c -g -fPIC -I$(LIBDEFUSE) -Wformat-truncation=0
CXXFLAGS=-c -g -fPIC -std=c++0x -I$(LIBDEFUSE) -Wformat-truncation=0
LFLAGS=-ldl -lrt -shared

OBJS = $(patsubst %.cpp,%.o,$(wildcard *.cpp)) $(patsubst %.c,%.o,$(wildcard *.c))

all: liblddefuse.so

cleanobjs:
	rm -f $(OBJS)

clean:
	rm -f $(OBJS)
	rm -f liblddefuse.so

.PHONY: libdefuse.a
libdefuse.a:
	$(MAKE) -C $(LIBDEFUSE)

liblddefuse.so: $(OBJS) libdefuse.a
	$(CXX) -o $@ $(OBJS) $(LFLAGS) $(LIBDEFUSE)/libdefuse.a

%.o: %.c
	$(CC) $(CFLAGS) -o $@ $<

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

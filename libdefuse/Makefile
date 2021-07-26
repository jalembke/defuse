CC=gcc
CXX=g++
AR=ar

CFLAGS=-c -g -fPIC -Wformat-truncation=0
CXXFLAGS=-c -g -fPIC -std=c++0x -Wformat-truncation=0
LFLAGS=-ldl -lrt -shared

OBJS = $(patsubst %.cpp,%.o,$(wildcard *.cpp)) $(patsubst %.c,%.o,$(wildcard *.c))

all: libdefuse.a

libdefuse.a: $(OBJS)
	$(AR) rcs $@ $(OBJS)

cleanobjs:
	rm -f $(OBJS)

clean:
	rm -f $(OBJS)
	rm -f libdefuse.a

%.o: %.c
	$(CC) $(CFLAGS) -o $@ $<

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

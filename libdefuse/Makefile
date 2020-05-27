CC=gcc
CXX=g++
AR=ar

CFLAGS=-c -g -fPIC
CXXFLAGS=-c -g -fPIC -std=c++0x
LFLAGS=-ldl -lrt -shared

OBJS = $(patsubst %.cpp,%.o,$(wildcard *.cpp)) $(patsubst %.c,%.o,$(wildcard *.c))

all: libdefuse.so

libdefuse.so: $(OBJS)
	$(CC) -o $@ $(OBJS) $(LFLAGS)

cleanobjs:
	rm -f $(OBJS)

clean:
	rm -f $(OBJS)
	rm -f libdefuse.so

%.o: %.c
	$(CC) $(CFLAGS) -o $@ $<

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

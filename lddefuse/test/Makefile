CC=gcc
CXX=g++
AR=ar

CFLAGS=-c -g -I../../src -I../
CXXFLAGS=-c -g -I../../src -I../
LFLAGS=-lrt

TESTS = $(patsubst %.cpp,test%,$(wildcard *.cpp)) $(patsubst %.c,test%,$(wildcard *.c))

all: $(TESTS)

clean:
	rm -f *.o
	rm -f $(TESTS)

%.o: %.c
	$(CC) $(CFLAGS) -o $@ $<

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

test%: %.o
	$(CXX) $(LFLAGS) -o $@ $<

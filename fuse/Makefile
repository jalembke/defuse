all: fusexmp fusetest

fusexmp:
	gcc -Wall fusexmp.c `pkg-config fuse --cflags --libs` -o fusexmp

fusetest:
	g++ -pthread -o fusetest -std=c++11 fusetest.cpp

clean:
	rm -f fusexmp fusetest

all: fusexmp fusetest

fusexmp: fusexmp.c
	gcc -Wall fusexmp.c `pkg-config fuse --cflags --libs` -o fusexmp

fusetest: fusetest.cpp
	g++ -pthread -o fusetest -std=c++11 fusetest.cpp

clean:
	rm -f fusexmp fusetest

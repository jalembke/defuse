all: fusexmp fusetest doCopy

doCopy: doCopy.cpp
	g++ -g -o doCopy -std=c++11 doCopy.cpp

fusexmp: fusexmp.c
	gcc -Wall fusexmp.c `pkg-config fuse --cflags --libs` -o fusexmp

fusetest: fusetest.cpp
	g++ -g -pthread -o fusetest -std=c++11 fusetest.cpp

clean:
	rm -f fusexmp fusetest doCopy

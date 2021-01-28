clean:
	rm -rf build proto/*.[ch]

.DEFAULT_GOAL = all
.PHONY : proto

build:
	mkdir build

build/proto: proto/*.proto build
	protoc-c --proto_path proto --c_out proto proto/*.proto
	touch build/proto

CCFLAGS := $(shell pkg-config --cflags --libs fuse libprotobuf-c) -Wall -Wextra -I. -std=gnu99 -fPIC

build/libhdfs_wrap.so: *.c *.h build/proto build
	mkdir -p build
	$(CC) -o build/libhdfs_wrap.so proto/*.c *.c $(CCFLAGS) -shared

all: CC += -DNDEBUG
all: build/libhdfs_wrap.so

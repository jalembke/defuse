#CC=musl-gcc
CC=gcc
SYSIO_HOME=../../direct-fuse
SYSIO_INCLUDE=$(SYSIO_HOME)/include

all: doTest doCopy doExecTest randUserFault userFault randmmap testmmap mmaptest #javaWrite.class javaOutputStream.class

doCopy: doCopy.c doCat
	${CC} -g -o doCopy doCopy.c

doCat: doCat.c
	${CC} -g -o doCat doCat.c

testread: testread.c
	${CC} -g -o testread testread.c

testwrite: testwrite.c
	${CC} -g -o testwrite testwrite.c

testmmap: testmmap.c
	${CC} -g -o testmmap testmmap.c

randmmap: randmmap.c
	${CC} -g -o randmmap randmmap.c

doExecTest: doExecTest.c doCat
	${CC} -g -o doExecTest doExecTest.c

doTest: doTest.c testread testwrite
	${CC} -g -pthread -o doTest doTest.c

doTestDirectFuse: doTestDirectFuse.c testread testwrite
	$(CC) -I$(SYSIO_INCLUDE) -g -pthread -o doTestDirectFuse doTestDirectFuse.c -L$(SYSIO_HOME) -lsysio `pkg-config --libs fuse`

mamptest: mmaptest.c
	${CC} -g -o mmaptest mmaptest.c

userFault: userFault.c
	${CC} -g -o userFault userFault.c -lrt -lpthread

randUserFault: randUserFault.c
	${CC} -g -o randUserFault randUserFault.c -lrt -lpthread

javaWrite.class: javaWrite.java
	javac javaWrite.java

javaOutputStream.class: javaOutputStream.java
	javac javaOutputStream.java

clean:
	rm -f doTest doCopy doCat doExecTest testread testwrite testmmap mmaptest javaWrite.class javaOutputStream.class userFault

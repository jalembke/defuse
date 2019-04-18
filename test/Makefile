CC=musl-gcc
#CC=gcc

all: doTest doCopy doExecTest userFault #javaWrite.class javaOutputStream.class

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

doExecTest: doExecTest.c doCat
	${CC} -g -o doExecTest doExecTest.c

doTest: doTest.c testread testwrite
	${CC} -g -pthread -o doTest doTest.c

userFault: userFault.c
	${CC} -g -o userFault userFault.c -lrt -lpthread

javaWrite.class: javaWrite.java
	javac javaWrite.java

javaOutputStream.class: javaOutputStream.java
	javac javaOutputStream.java

clean:
	rm -f doTest doCopy doCat doExecTest testread testwrite javaWrite.class javaOutputStream.class userFault

all: doTest doCopy doExecTest userFault #javaWrite.class javaOutputStream.class

doCopy: doCopy.c doCat
	musl-gcc -g -o doCopy doCopy.c

doCat: doCat.c
	musl-gcc -g -o doCat doCat.c

testread: testread.c
	musl-gcc -g -o testread testread.c

testwrite: testwrite.c
	musl-gcc -g -o testwrite testwrite.c

doExecTest: doExecTest.c doCat
	musl-gcc -g -o doExecTest doExecTest.c

doTest: doTest.c testread testwrite
	musl-gcc -g -pthread -o doTest doTest.c

userFault: userFault.c
	gcc -g -o userFault userFault.c -lrt -lpthread

javaWrite.class: javaWrite.java
	javac javaWrite.java

javaOutputStream.class: javaOutputStream.java
	javac javaOutputStream.java

clean:
	rm -f doTest doCopy doCat doExecTest testread testwrite javaWrite.class javaOutputStream.class userFault

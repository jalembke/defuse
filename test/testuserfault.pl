#!/usr/bin/perl

use strict;

#my $file_path = "/mnt/userfault";
my $file_path = "/tmp/userfault";

system("sudo rm -f $file_path");
system("sudo dd if=/dev/urandom of=$file_path bs=32768 count=4096");
system("sudo chown lembkej.Rosco $file_path");
system("sudo chmod 666 $file_path");

for(0 .. 50) {
	system("./userFault $file_path");
}

for(0 .. 50) {
	system("./testmmap $file_path");
}

system("fusermount -u /tmp/fuse");
system("../fuse/fusexmp /tmp/fuse");
for(0 .. 50) {
	system("./testmmap /tmp/fuse$file_path");
}
system("fusermount -u /tmp/fuse");

#!/usr/bin/perl

use strict;

use List::Util qw(sum);

my $direct_path = "/tmp/fusetest";
my $fuse_mount_path = "/tmp/tmpxmp";
my $fuse_path = "/tmp/tmpxmp/tmp/fusetest";

my $fusemount = $ENV{'HOME'} . "/ProxyFS/fuse/fusexmp";
my $fusetest = $ENV{HOME} . "/ProxyFS/fuse/fusetest";
my $fusermount = "/bin/fusermount";

my $trials = 20;
my $threads = 20;
my $files = 400;
my $nsec_per_sec = 1000000000;

sub doSystem {
	my $continue_on_error = shift;
	print "@_\n";
	unless(system(@_) == 0) {
		if($continue_on_error) {
			print STDERR "system @_ failed: $?\n";
		} else {
			die "system @_ failed: $?\n";
		}
	}
}

sub runTrials {
	my $target_path = shift;
	my @results;
	for(1..$trials) {
		doSystem(0, "rm -f $target_path/*");
		my $trial_result = `$fusetest $target_path $threads $files`;
		chomp $trial_result;
		push @results, $trial_result;
	}
	doSystem(0, "rm -f $target_path/*");
	return sum(@results)/$trials;
}

my $total_files = $threads * $files;

# Unmount the fuse mount
doSystem(1, "$fusermount -u $fuse_mount_path");
my $direct_avg = runTrials($direct_path);
my $direct_files_sec = $total_files / ($direct_avg / $nsec_per_sec);

doSystem(0, "$fusemount $fuse_mount_path");
my $fuse_avg = runTrials($fuse_path);
my $fuse_files_sec = $total_files / ($fuse_avg / $nsec_per_sec);
system("$fusermount -u $fuse_mount_path");

print "$direct_files_sec $fuse_files_sec\n";

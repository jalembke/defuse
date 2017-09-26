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

# Unmount the fuse mount
doSystem(1, "$fusermount -u $fuse_mount_path");
my $direct_avg = runTrials($direct_path);

doSystem(0, "$fusemount $fuse_mount_path");
my $fuse_avg = runTrials($fuse_path);
system("$fusermount -u $fuse_mount_path");

print "$direct_avg $fuse_avg\n";

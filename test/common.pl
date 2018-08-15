#!/usr/bin/perl

use strict;

sub in_set {
	my $value = shift;
	my %set = map {$_ => 1} @_;
	return defined($set{$value});
}

use constant {
	STOP_ON_ERROR => 0,
	CONTINUE_ON_ERROR => 1,
	NO_PRINT_COMMAND => 0,
	PRINT_COMMAND => 1,
};

my $print_command = 0;

sub _do_system {
	my $continue_on_error = shift;
	if($print_command == 1) {
		print "@_\n";
	}
	my $result = `@_`;
	unless($? == 0) {
		if($continue_on_error) {
			print STDERR "system @_ failed: $?\n";
		} else {
			die "system @_ failed: $?\n";
		}
	}
	return $result;
}

sub set_system_echo {
	$print_command = 1;
}

sub clear_system_echo {
	$print_command = 0;
}

sub system_or_die {
	return _do_system(STOP_ON_ERROR, @_);
}

sub system_or_continue {
	return _do_system(CONTINUE_ON_ERROR, @_);
}



return 1;

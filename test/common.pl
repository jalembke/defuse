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

sub _do_system {
	my $continue_on_error = shift;
	my $do_print = shift;
	if($do_print == 1) {
		print "@_\n";
	}
	unless(system(@_) == 0) {
		if($continue_on_error) {
			print STDERR "system @_ failed: $?\n";
		} else {
			die "system @_ failed: $?\n";
		}
	}
}

sub system_print_or_die {
	_do_system(STOP_ON_ERROR, PRINT_COMMAND, @_);
}

sub system_print_continue {
	_do_system(CONTINUE_ON_ERROR, PRINT_COMMAND, @_);
}

sub system_or_die {
	_do_system(STOP_ON_ERROR, NO_PRINT_COMMAND, @_);
}

sub system_continue {
	_do_system(CONTINUE_ON_ERROR, NO_PRINT_COMMAND, @_);
}

return 1;

#!/usr/bin/perl

use strict;

sub trim { my $s = shift; $s =~ s/^\s+|\s+$//g; return $s };

open (my $fh, "<", "../glibc_ops.h") or die "Can't open < ../glibc_ops.h: $!";

print 
"void 
load_glibc_ops(void)
{
	static bool load_glibc_ops_flag = false;

	if(!load_glibc_ops_flag) {

		bzero(&real_ops, sizeof(struct glibc_ops));\n";

while(<$fh>) {
	if(/\s+(\S+) \(\*(\w+)\)/) {
		my $ret_type = $1;
		my $func = $2;
		my @arg_types = ();
		for(split(/,/, $')) {
			if(/(\.+)/) {
				push @arg_types, trim($1);
				next;
			}
			if(/([0-9a-zA-Z_ *]+)( |\*)\w+\[/) {
				push @arg_types, trim($1) . trim($2) . "*";
				next;
			}
			if(/([0-9a-zA-Z_ *]+)( |\*)\w+/) {
				push @arg_types, trim($1) . trim($2);
				next;
			}
		}
		printf("\t\treal_ops.%s = (%s (*)(%s))dlsym_exit_on_error(RTLD_NEXT, \"%s\");\n", $func, $ret_type, join(", ", @arg_types), $func);
	}
}

print 
"		load_glibc_ops_flag = true;
	}
}\n";

close $fh;

#!/usr/bin/perl

use strict;

use File::Basename;
use Cwd 'abs_path';
my $script_dir = dirname(abs_path($0));
my $src_dir = dirname($script_dir);

my $base_backend_mount = "/mnt";
my $base_wrapper_mount = "/tmp";
my $test_dir = "/test";
my $avfs_target = "/test.tar.gz";

my $fuse_mount_path = "$base_wrapper_mount/fuse";
my $direct_mount_path = $base_backend_mount;
my $usfsal_mount_path = "$base_wrapper_mount/usfsal";
my $usfsal_backend = "/";
our $direct_path = "$direct_mount_path$test_dir";
our $fuse_path = "$fuse_mount_path$direct_path";
our $usfsal_path = "$usfsal_mount_path$direct_path";

my $avfs_path = "$direct_path$avfs_target#/";
our $fuse_avfs_path = "$fuse_mount_path$avfs_path";
our $usfsal_avfs_path = "$usfsal_mount_path$avfs_path";

my $fusemount = "$src_dir/fuse/fusexmp_fh";
my $avfsfusemount = "$src_dir/fuse/avfsd";
my $fusermount = "/bin/fusermount";
my $usfsal_user_library = "$src_dir/usfs_wrap/libusfs_wrap.so";
our $ldpreload_library = "$src_dir/lddefuse/liblddefuse.so";
our $ldpreload_avfs_library = "$src_dir/lddefuse/liblddefuse.avfs.so";

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

sub exec_or_die {
	if($print_command == 1) {
		print "@_\n";
	}
	exec(@_);
	die "exec @_ failed: $!\n";
}

sub system_or_die {
	return _do_system(STOP_ON_ERROR, @_);
}

sub system_or_continue {
	return _do_system(CONTINUE_ON_ERROR, @_);
}

sub drop_caches {
	system_or_die("/bin/sync");
	sleep 1;
	#system_or_die("/bin/echo 1 | /usr/bin/sudo /usr/bin/tee /proc/sys/vm/drop_caches > /dev/null");
}

sub mount_frontend {
	my $type = shift;
	if($type eq "fuse") {
		system_or_die("$fusemount $fuse_mount_path");
	} elsif($type eq "fuse_direct") {
		system_or_die("$fusemount -o direct_io $fuse_mount_path");
	} elsif($type eq "usfsal") {
		system_or_die("/usr/bin/sudo /bin/mount -t bopfs -o backend=$usfsal_backend -o library=$usfsal_user_library bopfs $usfsal_mount_path");
	} elsif($type eq "avfs") {
		system_or_die("$avfsfusemount $fuse_mount_path");
	} elsif($type eq "avfs_direct") {
		system_or_die("$avfsfusemount -o direct_io $fuse_mount_path");
	}
}

sub umount_frontend {
	my $type = shift;
	if($type eq "fuse") {
		system_or_die("$fusermount -u $fuse_mount_path");
	} elsif($type eq "fuse_direct") {
		system_or_die("$fusermount -u $fuse_mount_path");
	} elsif($type eq "usfsal") {
		system_or_die("/usr/bin/sudo /bin/umount $usfsal_mount_path");
	} elsif($type eq "avfs") {
		system_or_die("$fusermount -u $fuse_mount_path");
	} elsif($type eq "avfs_direct") {
		system_or_die("$fusermount -u $fuse_mount_path");
	}
}

sub copy_avfs_target {
	my $source_file = shift;
	system_or_die("cp $src_dir/test/$source_file $direct_path$avfs_target");
}

sub reset_mounts {
	my $backend_dev = shift;
	my $backend_type = shift;

	# Unmount FUSE
	system_or_continue("$fusermount -u $fuse_mount_path");

	# Unmount usfsal
	system_or_continue("/usr/bin/sudo /bin/umount $usfsal_mount_path");

	# Reset mount point contents
	system_or_continue("/bin/rm -rf $fuse_mount_path");
	system_or_continue("/bin/mkdir -p $fuse_mount_path");
	system_or_continue("/bin/rm -rf $usfsal_mount_path");
	system_or_continue("/bin/mkdir -p $usfsal_mount_path");

	# Remove usfsal shared spaces
	system_or_continue("/bin/rm -rf /tmp/usfsal_*");

	# Reset system mounts for backend file systems
	system_or_continue("/usr/bin/sudo /bin/umount $direct_mount_path");

	my $mount_options = "";
	if($backend_type eq "vfat") {
		$mount_options = "-o rw,uid=\$(id -u),gid=\$(id -g)";
	} elsif($backend_type eq "tmpfs") {
		$mount_options = "-t tmpfs -o size=768m"
	}
	system_or_die("/usr/bin/sudo /bin/mount $mount_options $backend_dev $direct_mount_path");
	system_or_die("/usr/bin/sudo /bin/mkdir -p $direct_path");
	system_or_die("/usr/bin/sudo /bin/chmod 777 $direct_path");
}

return 1;

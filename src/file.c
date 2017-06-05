/*
  ProxyFS: Proxy File System
  Copyright (C) 2017  James Lembke <jalembke@gmail.com>

  This program can be distributed under the terms of the GNU GPL.
  See the file COPYING.
*/

#include "proxyfs.h"

#include <linux/fs.h>
#include <linux/namei.h>

static ssize_t proxyfs_read(struct file *file, char __user *buf, size_t count, loff_t *offset) {
	printk(KERN_INFO "%s\n", __PRETTY_FUNCTION__);
	return 0;
}

static ssize_t proxyfs_write(struct file *file, const char __user *buf, size_t count, loff_t *offset) {
	printk(KERN_INFO "%s\n", __PRETTY_FUNCTION__);
	return 0;
}

static int proxyfs_open(struct inode *inode, struct file *file) {
	printk(KERN_INFO "%s\n", __PRETTY_FUNCTION__);
	return 0;
}

static int proxyfs_flush(struct file *file, fl_owner_t id) {
	printk(KERN_INFO "%s\n", __PRETTY_FUNCTION__);
	return 0;
}

static int proxyfs_release(struct inode *inode, struct file *file) {
	printk(KERN_INFO "%s\n", __PRETTY_FUNCTION__);
	return 0;
}

static int proxyfs_fsync(struct file *file, loff_t start, loff_t end, int datasync) {
	printk(KERN_INFO "%s\n", __PRETTY_FUNCTION__);
	return 0;
}

static long proxyfs_fallocate(struct file *file, int mode, loff_t offset, loff_t len) {
	printk(KERN_INFO "%s\n", __PRETTY_FUNCTION__);
	return 0;
}

const struct file_operations proxy_file_operations = {
	.read		= proxyfs_read,
	.write		= proxyfs_write,
	.open		= proxyfs_open,
	.flush		= proxyfs_flush,
	.release	= proxyfs_release,
	.fsync		= proxyfs_fsync,
	.fallocate	= proxyfs_fallocate,
};

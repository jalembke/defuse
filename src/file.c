/*
  ProxyFS: Proxy File System
  Copyright (C) 2017  James Lembke <jalembke@gmail.com>

  This program can be distributed under the terms of the GNU GPL.
  See the file COPYING.
*/

#include "proxyfs.h"

#include <linux/module.h>
#include <linux/fs.h>
#include <linux/namei.h>

static inline int proxyfs_resolve_reopen(struct file* file)
{
	struct dentry* b_dentry;
	struct inode* b_inode;
	struct dentry *entry = file->f_path.dentry;
	struct inode* inode = entry->d_inode;
	int opened = 0;
	int err;

	b_dentry = proxyfs_get_b_dentry_resolved(inode);
	if(IS_ERR(b_dentry))
		return PTR_ERR(b_dentry);

	b_inode = d_inode(b_dentry);
	if(!b_inode)
		return -ENOENT;

	file->f_path.mnt = proxyfs_get_b_mount(inode);
	file->f_path.dentry = b_dentry;

	err = finish_open(file, b_dentry, NULL, &opened);
	if(err)
		return err;

	fops_put(file->f_op);

	return 0;
}

static ssize_t proxyfs_read(struct file *file, char __user *buf, size_t count, loff_t *offset)
{
	int err;
	
	printk(KERN_INFO "%s\n", __PRETTY_FUNCTION__);
	err = proxyfs_resolve_reopen(file);
	if(err)
		return err;
	
	return vfs_read(file, buf, count, offset);
}

static ssize_t proxyfs_write(struct file *file, const char __user *buf, size_t count, loff_t *offset) {
	printk(KERN_INFO "%s\n", __PRETTY_FUNCTION__);
	return 0;
}

static int proxyfs_open(struct inode *inode, struct file *file)
{
	int err;
	
	printk(KERN_INFO "%s\n", __PRETTY_FUNCTION__);

	err = generic_file_open(inode, file);
	if (err)
		return err;

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

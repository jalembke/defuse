/*
  ProxyFS: Proxy File System
  Copyright (C) 2017  James Lembke <jalembke@gmail.com>

  This program can be distributed under the terms of the GNU GPL.
  See the file COPYING.
*/

#include "proxyfs.h"

#include <linux/module.h>
#include <linux/fs.h>
#include <linux/file.h>
#include <linux/namei.h>
#include <linux/cred.h>
#include <linux/sched.h>

static inline struct file* get_proxyfs_b_file(struct file* file)
{
	return file->private_data;
}

static inline void set_proxyfs_b_file(struct file* file, struct file* b_file)
{
	file->private_data = b_file;
}

static inline bool proxyfs_resolved_file(struct file* file)
{
	return (file ? (get_proxyfs_b_file(file)) != NULL : false);
}

static inline int proxyfs_resolve_reopen(struct file* file)
{
	struct dentry* b_dentry;
	struct inode* b_inode;
	struct file* b_file;
	struct dentry *entry = file->f_path.dentry;
	struct inode* inode = entry->d_inode;
	struct path b_path;

	// Resolve backend inodes
	b_dentry = proxyfs_get_b_dentry_resolved(inode);
	if(IS_ERR(b_dentry))
		return PTR_ERR(b_dentry);

	b_inode = d_inode(b_dentry);
	if(!b_inode)
		return -ENOENT;

	// Open the file in the backend file system
	b_path.mnt = proxyfs_get_b_mount(inode);
	b_path.dentry = b_dentry;
	b_file = dentry_open(&b_path, file->f_flags, current_cred());

	if(IS_ERR(b_file))
		return PTR_ERR(b_file);

	// File backend file successfully opened, set the pointer and return
	set_proxyfs_b_file(file, b_file);

	return 0;
}

static inline struct file* proxyfs_get_b_file_resolved(struct file* file)
{
	int err;
	struct file* b_file;

	if(!file)
		return ERR_PTR(-ENOENT);

	if(!proxyfs_resolved_file(file)) {
		err = proxyfs_resolve_reopen(file);
		if(err)
			return ERR_PTR(err);
	}
	b_file = get_proxyfs_b_file(file);
	
	if(!b_file)
		return ERR_PTR(-ENOENT);

	return b_file;
}

static ssize_t proxyfs_read(struct file *file, char __user *buf, size_t count, loff_t *offset)
{
	struct file* b_file;
	
	printk(KERN_INFO "%s\n", __PRETTY_FUNCTION__);

	b_file = proxyfs_get_b_file_resolved(file);
	if(IS_ERR(b_file))
		return PTR_ERR(b_file);
	
	return vfs_read(b_file, buf, count, offset);
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

	file->private_data = NULL;

	return 0;
}

static int proxyfs_flush(struct file *file, fl_owner_t id) {
	printk(KERN_INFO "%s\n", __PRETTY_FUNCTION__);
	return 0;
}

static int proxyfs_release(struct inode *inode, struct file *file)
{
	printk(KERN_INFO "%s\n", __PRETTY_FUNCTION__);

	if(proxyfs_resolved_file(file)) {
		fput(get_proxyfs_b_file(file));
	}

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

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
	
	printk(KERN_INFO "%s\n", __PRETTY_FUNCTION__);

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

static loff_t proxyfs_llseek(struct file *file, loff_t offset, int whence)
{
	loff_t retval;
	struct inode *inode;
	struct inode *b_inode;

	PRINTFN;

	inode = file->f_path.dentry->d_inode;
	mutex_lock(&inode->i_mutex);
	switch (whence) {
		// If seeking from the end, retrieve the backend inode
		//    to know the file size
		case SEEK_END:
			b_inode = proxyfs_get_b_inode_resolved(inode);
			if(IS_ERR(b_inode)) {
			   mutex_unlock(&inode->i_mutex);
			   return PTR_ERR(b_inode);
			}
			offset += i_size_read(b_inode);
			break;
		// If seeking from the current position
		case SEEK_CUR:
			offset += file->f_pos;
			break;
	}
	retval = -EINVAL;
	if (offset >= 0) {
		if (offset != file->f_pos) {
			file->f_pos = offset;
		}
		retval = offset;
	}
	mutex_unlock(&inode->i_mutex);
	return retval;
}

static ssize_t proxyfs_read(struct file *file, char __user *buf, size_t count, loff_t *offset)
{
	struct file* b_file;
	
	PRINTFN;

	b_file = proxyfs_get_b_file_resolved(file);
	if(IS_ERR(b_file))
		return PTR_ERR(b_file);
	
	return vfs_read(b_file, buf, count, offset);
}

static ssize_t proxyfs_write(struct file *file, const char __user *buf, size_t count, loff_t *offset)
{
	struct file* b_file;
	
	PRINTFN;

	b_file = proxyfs_get_b_file_resolved(file);
	if(IS_ERR(b_file))
		return PTR_ERR(b_file);

	return vfs_write(b_file, buf, count, offset);
}

static int proxyfs_open(struct inode *inode, struct file *file)
{
	int err;
	
	PRINTFN;
	
	err = generic_file_open(inode, file);
	if (err)
		return err;

	file->private_data = NULL;

	return 0;
}

static int proxyfs_flush(struct file *file, fl_owner_t id) 
{
	struct file* b_file;
	
	PRINTFN;

	if(proxyfs_resolved_file(file)) {
		b_file = get_proxyfs_b_file(file);
		if(b_file->f_op->flush)
			return b_file->f_op->flush(b_file, id);
	}

	return 0;
}

static int proxyfs_release(struct inode *inode, struct file *file)
{
	PRINTFN;

	if(proxyfs_resolved_file(file)) {
		fput(get_proxyfs_b_file(file));
	}

	return 0;
}

static int proxyfs_fsync(struct file *file, loff_t start, loff_t end, int datasync)
{
	struct file* b_file;
	
	PRINTFN;

	b_file = proxyfs_get_b_file_resolved(file);
	if(IS_ERR(b_file))
		return PTR_ERR(b_file);
	
	return vfs_fsync_range(b_file, start, end, datasync);
}

static int proxyfs_fasync(int fd, struct file *file, int flags)
{
	struct file* b_file;
	
	PRINTFN;

	b_file = proxyfs_get_b_file_resolved(file);
	if(IS_ERR(b_file))
		return PTR_ERR(b_file);

	if(b_file->f_op->fasync)
		return b_file->f_op->fasync(fd, b_file, flags);
	
	return 0;
}

static long proxyfs_fallocate(struct file *file, int mode, loff_t offset, loff_t len)
{
	struct file* b_file;
	
	PRINTFN;

	b_file = proxyfs_get_b_file_resolved(file);
	if(IS_ERR(b_file))
		return PTR_ERR(b_file);

	return vfs_fallocate(b_file, mode, offset, len);
}

const struct file_operations proxy_file_operations = {
	.llseek		= proxyfs_llseek,
	.read		= proxyfs_read,
	.write		= proxyfs_write,
	.open		= proxyfs_open,
	.flush		= proxyfs_flush,
	.release	= proxyfs_release,
	.fsync		= proxyfs_fsync,
	.fasync		= proxyfs_fasync,
	.fallocate	= proxyfs_fallocate,
};

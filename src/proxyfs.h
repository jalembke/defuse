/*
  PROXYFS: Proxy Filesystem
  Copyright (C) 2017  James Lembke <jalembke@gmail.com>

  This program can be distributed under the terms of the GNU GPL.
  See the file COPYING.
*/

#ifndef _FS_PROXYFS_H
#define _FS_PROXYFS_H

#include <linux/fs.h>

/** PROXYFS inode */
struct proxyfs_inode {
	/** Inode data */
	struct inode inode;

	/** Proxy dentry */
	struct dentry *p_dentry;

	/** Backend dentry */
	struct dentry *b_dentry;
};

/** PROXYFS mount options */
struct proxyfs_mount_opts {
	umode_t mode;
	char backend[PATH_MAX];
};

/** PROXYFS file system info */
struct proxyfs_fs_info {
	struct proxyfs_mount_opts mount_opts;
};

struct inode* proxyfs_iget(struct super_block *sb, struct inode *dir, struct dentry *entry, mode_t mode);

static inline struct proxyfs_inode *get_proxyfs_inode(struct inode *inode)
{
	return container_of(inode, struct proxyfs_inode, inode);
}

static inline bool proxyfs_resolved_inode(struct inode *inode)
{
	return (get_proxyfs_inode(inode)->b_dentry != NULL);
}

extern const struct inode_operations proxy_inode_operations;
extern const struct file_operations proxy_file_operations;

#endif // _FS_PROXYFS_H

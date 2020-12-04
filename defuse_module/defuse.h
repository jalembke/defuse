/*
  DEFUSE File System
  Copyright (C) 2020  James Lembke <jalembke@gmail.com>

  This program can be distributed under the terms of the GNU GPL.
  See the file COPYING.
*/

#ifndef _FS_DEFUSE_H
#define _FS_DEFUSE_H

#include <linux/fs.h>
#include <linux/dcache.h>

struct vfsmount;

#ifdef DEBUG_DEFUSE
#define PRINTFN printk(KERN_INFO "%s\n", __PRETTY_FUNCTION__);
#else
#define PRINTFN
#endif

/** DEFUSE inode */
struct defuse_inode {
	/** Inode data */
	struct inode inode;

	/** defuse dentry */
	struct dentry *p_dentry;
};

/** DEFUSE mount options */
struct defuse_mount_opts {
	umode_t mode;
	char* backend_path;
	char* user_library_path;
};

/** DEFUSE file system info */
struct defuse_fs_info {
	struct defuse_mount_opts mount_opts;
	struct vfsmount* b_mount;
};

static inline struct vfsmount* defuse_get_b_mount(struct inode* inode)
{
	struct super_block *sb = inode->i_sb;
	struct defuse_fs_info *fsi = (struct defuse_fs_info*)sb->s_fs_info;
	return fsi->b_mount;
}

static inline struct defuse_inode *get_defuse_inode(struct inode *inode)
{
	return container_of(inode, struct defuse_inode, inode);
}

static inline struct dentry* defuse_get_p_dentry(struct inode* inode)
{
	return get_defuse_inode(inode)->p_dentry;
}

#endif // _FS_DEFUSE_H

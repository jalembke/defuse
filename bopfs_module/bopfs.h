/*
  BOPFS Filesystem
  Copyright (C) 2017  James Lembke <jalembke@gmail.com>

  This program can be distributed under the terms of the GNU GPL.
  See the file COPYING.
*/

#ifndef _FS_BOPFS_H
#define _FS_BOPFS_H

#include <linux/fs.h>
#include <linux/dcache.h>

struct vfsmount;

#ifdef DEBUG_BOPFS
#define PRINTFN printk(KERN_INFO "%s\n", __PRETTY_FUNCTION__);
#else
#define PRINTFN
#endif

/** BOPFS inode */
struct bopfs_inode {
	/** Inode data */
	struct inode inode;

	/** bopfs dentry */
	struct dentry *p_dentry;
};

/** BOPFS mount options */
struct bopfs_mount_opts {
	umode_t mode;
	char* backend_path;
	char* user_library_path;
};

/** BOPFS file system info */
struct bopfs_fs_info {
	struct bopfs_mount_opts mount_opts;
	struct vfsmount* b_mount;
};

static inline struct vfsmount* bopfs_get_b_mount(struct inode* inode)
{
	struct super_block *sb = inode->i_sb;
	struct bopfs_fs_info *fsi = (struct bopfs_fs_info*)sb->s_fs_info;
	return fsi->b_mount;
}

static inline struct bopfs_inode *get_bopfs_inode(struct inode *inode)
{
	return container_of(inode, struct bopfs_inode, inode);
}

static inline struct dentry* bopfs_get_p_dentry(struct inode* inode)
{
	return get_bopfs_inode(inode)->p_dentry;
}

#endif // _FS_BOPFS_H

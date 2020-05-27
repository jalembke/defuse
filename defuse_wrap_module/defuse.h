/*
  DEFUSE Wrapper Module Filesystem
  Copyright (C) 2017  James Lembke <jalembke@gmail.com>

  Wrap backend file system with a delayed lookup

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

	/** Proxy dentry */
	struct dentry *p_dentry;

	/** Backend dentry */
	struct dentry *b_dentry;
};

/** DEFUSE mount options */
struct defuse_mount_opts {
	umode_t mode;
	char* backend;
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

static inline struct dentry* defuse_get_b_dentry(struct inode* inode)
{
	return get_defuse_inode(inode)->b_dentry;
}

static inline struct inode* defuse_get_b_inode(struct inode* inode)
{
	return d_inode(defuse_get_b_dentry(inode));
}

static inline bool defuse_resolved_inode(struct inode *inode)
{
	return (inode ? (get_defuse_inode(inode)->b_dentry != NULL) : true);
}

struct dentry* defuse_resolve_dentry(struct dentry* entry);

static inline struct dentry* defuse_get_b_dentry_resolved(struct inode* inode)
{
	struct dentry* b_dentry;

	if(!inode)
		return ERR_PTR(-ENOENT);

	/* 
	 * Get the backend dentry either through a resolve
	 * or directly from the inode
	 */
	if(!defuse_resolved_inode(inode)) {
		b_dentry = defuse_resolve_dentry(defuse_get_p_dentry(inode));
	} else {
		b_dentry = defuse_get_b_dentry(inode);
	}

	/* If the backend is negative return ENOENT */
	if(!b_dentry || !b_dentry->d_inode)
		return ERR_PTR(-ENOENT);

	return b_dentry;
}

static inline struct inode* defuse_get_b_inode_resolved(struct inode* inode)
{
	struct dentry* b_dentry = defuse_get_b_dentry_resolved(inode);
	int err = PTR_ERR(b_dentry);
	if(!b_dentry) 
		return ERR_PTR(-ENOENT);

	if(err)
		return ERR_PTR(err);
	
	return d_inode(b_dentry);
}

extern const struct inode_operations proxy_inode_operations;
extern const struct file_operations proxy_file_operations;

#endif // _FS_DEFUSE_H

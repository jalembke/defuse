/*
  PROXYFS: Proxy Filesystem
  Copyright (C) 2017  James Lembke <jalembke@gmail.com>

  This program can be distributed under the terms of the GNU GPL.
  See the file COPYING.
*/

#ifndef _FS_PROXYFS_H
#define _FS_PROXYFS_H

#include <linux/fs.h>
#include <linux/dcache.h>

struct vfsmount;

#ifdef DEBUG_PROXYFS
#define PRINTFN printk(KERN_INFO "%s\n", __PRETTY_FUNCTION__);
#else
#define PRINTFN
#endif

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
	char* backend;
};

/** PROXYFS file system info */
struct proxyfs_fs_info {
	struct proxyfs_mount_opts mount_opts;
	struct vfsmount* b_mount;
};

static inline struct vfsmount* proxyfs_get_b_mount(struct inode* inode)
{
	struct super_block *sb = inode->i_sb;
	struct proxyfs_fs_info *fsi = (struct proxyfs_fs_info*)sb->s_fs_info;
	return fsi->b_mount;
}

static inline struct proxyfs_inode *get_proxyfs_inode(struct inode *inode)
{
	return container_of(inode, struct proxyfs_inode, inode);
}

static inline struct dentry* proxyfs_get_p_dentry(struct inode* inode)
{
	return get_proxyfs_inode(inode)->p_dentry;
}

static inline struct dentry* proxyfs_get_b_dentry(struct inode* inode)
{
	return get_proxyfs_inode(inode)->b_dentry;
}

static inline struct inode* proxyfs_get_b_inode(struct inode* inode)
{
	return d_inode(proxyfs_get_b_dentry(inode));
}

static inline bool proxyfs_resolved_inode(struct inode *inode)
{
	return (inode ? (get_proxyfs_inode(inode)->b_dentry != NULL) : true);
}

struct dentry* proxyfs_resolve_dentry(struct dentry* entry);

static inline struct dentry* proxyfs_get_b_dentry_resolved(struct inode* inode)
{
	struct dentry* b_dentry;

	if(!inode)
		return ERR_PTR(-ENOENT);

	/* 
	 * Get the backend dentry either through a resolve
	 * or directly from the inode
	 */
	if(!proxyfs_resolved_inode(inode)) {
		b_dentry = proxyfs_resolve_dentry(proxyfs_get_p_dentry(inode));
	} else {
		b_dentry = proxyfs_get_b_dentry(inode);
	}

	/* If the backend is negative return ENOENT */
	if(!b_dentry || !b_dentry->d_inode)
		return ERR_PTR(-ENOENT);

	return b_dentry;
}

static inline struct inode* proxyfs_get_b_inode_resolved(struct inode* inode)
{
	struct dentry* b_dentry = proxyfs_get_b_dentry_resolved(inode);
	int err = PTR_ERR(b_dentry);
	if(!b_dentry) 
		return ERR_PTR(-ENOENT);

	if(err)
		return ERR_PTR(err);
	
	return d_inode(b_dentry);
}

extern const struct inode_operations proxy_inode_operations;
extern const struct file_operations proxy_file_operations;

#endif // _FS_PROXYFS_H

/*
  ProxyFS: Proxy File System
  Copyright (C) 2017  James Lembke <jalembke@gmail.com>

  This program can be distributed under the terms of the GNU GPL.
  See the file COPYING.
*/

#include "proxyfs.h"

#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/namei.h>
#include <linux/pagemap.h>
#include <linux/time.h>
#include <linux/init.h>
#include <linux/string.h>
#include <linux/parser.h>
#include <linux/slab.h>
#include <linux/mount.h>
#include <asm/uaccess.h>
#include <linux/xattr.h>
#include <linux/fs_stack.h>

MODULE_AUTHOR("James Lembke <jalembke@gmail.com>");
MODULE_DESCRIPTION("Proxy File System");
MODULE_LICENSE("GPL");

#define PROXYFS_DEFAULT_MODE	0755
#define PROXYFS_MAGIC			0x50525859	// PRXY

static struct kmem_cache *proxyfs_inode_cachep;

static struct inode* proxyfs_iget(struct super_block *sb, struct inode *dir, struct dentry *entry, mode_t mode);

/* 
 * Resolve the given proxy dentry to the real backend dentry
 *
 * Returns -
 *   Backend dentry - If lookup succeeds
 *   Negative dentry - If entry does not exist on the backend
 *   NULL - on error
 *
 * NOTE: this modifies the entry as well as the parent entri(es) 
 */
struct dentry* proxyfs_resolve_dentry(struct dentry* entry)
{
	struct dentry *p_parent;
	struct dentry *b_parent;
	struct dentry *b_dentry;

	printk(KERN_INFO "%s\n", __PRETTY_FUNCTION__);

	if(!entry)
		return NULL;

	/* Retreive the parent dentry */
	p_parent = dget_parent(entry);
	if(!p_parent)
		return NULL;
	if(!p_parent->d_inode) {
		dput(p_parent);
		return NULL;
	}

	/* Resolve the parent if it hasn't been already */
	if(!proxyfs_resolved_inode(p_parent->d_inode)) {
		b_parent = proxyfs_resolve_dentry(p_parent);
		if(!b_parent) {
			dput(p_parent);
			return NULL;
		}
		get_proxyfs_inode(p_parent->d_inode)->b_dentry = b_parent;
	} else {
		b_parent = proxyfs_get_b_dentry(p_parent->d_inode);
	}

	/* Parent's backend dentry is negative, the child backend entry should also be negative */
	if(!b_parent->d_inode) {
		dput(p_parent);
		return entry;
	}

	/* Resolve the child based on the proxy path and the backend parent dentry */
	inode_lock(b_parent->d_inode);
	b_dentry = lookup_one_len(entry->d_name.name, b_parent, entry->d_name.len);
	inode_unlock(b_parent->d_inode);
	
	if(IS_ERR(b_dentry)) {
		dput(p_parent);
		return NULL;
	}

	dput(p_parent);
	
	return b_dentry;
}

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

static inline int proxyfs_do_create(struct inode *dir, struct dentry *entry, umode_t mode, 
	dev_t dev, const char* link, struct dentry *hardlink)
{
	struct inode *inode;
	struct dentry *b_dir;
	struct dentry *b_dentry;
	struct dentry *b_hardlink;
	int err;

	b_dir = proxyfs_get_b_dentry_resolved(dir);
	err = PTR_ERR(b_dir);
	if(IS_ERR(b_dir)) {
		return err;
	}
	if(hardlink) {
		b_hardlink = proxyfs_get_b_dentry_resolved(d_inode(hardlink));
		err = PTR_ERR(b_hardlink);
		if(IS_ERR(b_hardlink)) {
			return err;
		}
	}
	b_dentry = proxyfs_resolve_dentry(entry);
	if(!b_dentry)
		return -EIO;
	if(d_inode(b_dentry)) {
		dput(b_dentry);
		return -ESTALE;
	}

	inode = proxyfs_iget(dir->i_sb, dir, entry, mode);
	if(!inode) {
		dput(b_dentry);
		return -ENOSPC;
	}

	mutex_lock_nested(&b_dir->d_inode->i_mutex, I_MUTEX_PARENT);
	if(hardlink) {
		err = vfs_link(b_hardlink, d_inode(b_dir), b_dentry, NULL);
	} else {
		switch (mode & S_IFMT) {
			case S_IFREG:
				err = vfs_create(d_inode(b_dir), b_dentry, mode, true);
				break;
			case S_IFDIR:
				err = vfs_mkdir(d_inode(b_dir), b_dentry, mode);
				break;
			case S_IFCHR:
			case S_IFBLK:
			case S_IFIFO:
			case S_IFSOCK:
				err = vfs_mknod(d_inode(b_dir), b_dentry, mode, dev);
				break;
			case S_IFLNK:
				err = vfs_symlink(d_inode(b_dir), b_dentry, link);
				break;
			default:
				err = -EPERM;
		}
	}
	mutex_unlock(&b_dir->d_inode->i_mutex);

	if(err) {
		dput(b_dentry);
	}

	d_instantiate(entry, inode);
	get_proxyfs_inode(inode)->b_dentry = b_dentry;
	return 0;
}

static inline int proxyfs_create_object(struct inode *dir, struct dentry *entry, umode_t mode, 
	dev_t dev, const char* link, struct dentry* hardlink)
{
	struct vfsmount* b_mnt;
	int err;

	b_mnt = proxyfs_get_b_mount(dir);
	err = mnt_want_write(b_mnt);
	if(err) {
		return err;
	}
	err = proxyfs_do_create(dir, entry, mode, dev, link, hardlink);
	mnt_drop_write(b_mnt);
	
	return err;
}

static inline int proxyfs_do_delete(struct inode *dir, struct dentry *entry)
{
	struct inode *inode;
	struct dentry *b_dir;
	struct dentry * b_dentry;
	int err;

	printk(KERN_INFO "%s\n", __PRETTY_FUNCTION__);

	b_dir = proxyfs_get_b_dentry_resolved(dir);
	err = PTR_ERR(b_dir);
	if(IS_ERR(b_dir)) {
		return err;
	}

	inode = d_inode(entry);
	b_dentry = proxyfs_get_b_dentry_resolved(inode);
	err = PTR_ERR(b_dentry);
	if(IS_ERR(b_dentry)) {
		return err;
	}

	mutex_lock_nested(&b_dir->d_inode->i_mutex, I_MUTEX_PARENT);
	if(!S_ISDIR(b_dentry->d_inode->i_mode)) {
		err = vfs_rmdir(d_inode(b_dir), b_dentry);
	} else {
		err = vfs_unlink(d_inode(b_dir), b_dentry, NULL);
	}
	mutex_unlock(&b_dir->d_inode->i_mutex);

	return err;
}

static inline int proxyfs_delete_object(struct inode *dir, struct dentry *entry)
{
	struct vfsmount* b_mnt;
	int err;

	b_mnt = proxyfs_get_b_mount(dir);
	err = mnt_want_write(b_mnt);
	if(err) {
		return err;
	}
	err = proxyfs_do_delete(dir, entry);
	mnt_drop_write(b_mnt);
	
	return err;
}

static int proxyfs_create(struct inode *dir, struct dentry *entry, umode_t mode,
			bool want_excl)
{
	printk(KERN_INFO "%s %08X\n", __PRETTY_FUNCTION__, mode);
	return proxyfs_create_object(dir, entry, mode, 0, NULL, NULL);
}

struct dentry *proxyfs_lookup(struct inode *dir, struct dentry *entry, unsigned int flags)
{
	struct inode *inode = NULL;
	struct dentry *b_dentry = NULL;
	mode_t base_mode = S_IFREG;
	
	printk(KERN_INFO "%s %s %08X %016llX\n", __PRETTY_FUNCTION__, entry->d_name.name, flags, (unsigned long long)entry->d_inode);

	if(flags & LOOKUP_DIRECTORY)
		base_mode = S_IFDIR;
	
	if(flags & LOOKUP_CREATE) {
		b_dentry = proxyfs_resolve_dentry(entry);
		if(!b_dentry)
			return ERR_PTR(-EIO);
		if(b_dentry->d_inode) {
			inode = proxyfs_iget(dir->i_sb, dir, entry, b_dentry->d_inode->i_mode);
			if(!inode) {
				dput(b_dentry);
				return ERR_PTR(-ENOSPC);
			}
			get_proxyfs_inode(inode)->b_dentry = b_dentry;
		} else {
			dput(b_dentry);
		}
	} else {
		inode = proxyfs_iget(dir->i_sb, dir, entry, base_mode | PROXYFS_DEFAULT_MODE);
		if(!inode)
			return ERR_PTR(-ENOSPC);
	}

	return d_splice_alias(inode, entry);
}

static int proxyfs_link(struct dentry *entry, struct inode *newdir,
		     struct dentry *newent)
{
	printk(KERN_INFO "%s\n", __PRETTY_FUNCTION__);
	return proxyfs_create_object(newdir, newent, entry->d_inode->i_mode, 0, NULL, entry);
}

static int proxyfs_unlink(struct inode *dir, struct dentry *entry)
{
	printk(KERN_INFO "%s\n", __PRETTY_FUNCTION__);
	return proxyfs_delete_object(dir, entry);
}

static int proxyfs_symlink(struct inode *dir, struct dentry *entry,
			const char *link)
{
	printk(KERN_INFO "%s\n", __PRETTY_FUNCTION__);
	return proxyfs_create_object(dir, entry, S_IFLNK, 0, link, NULL);
}

static int proxyfs_mkdir(struct inode *dir, struct dentry *entry, umode_t mode)
{
	printk(KERN_INFO "%s\n", __PRETTY_FUNCTION__);
	return proxyfs_create_object(dir, entry, mode, 0, NULL, NULL);
}

static int proxyfs_rmdir(struct inode *dir, struct dentry *entry)
{
	printk(KERN_INFO "%s\n", __PRETTY_FUNCTION__);
	return proxyfs_delete_object(dir, entry);
}

static int proxyfs_mknod(struct inode *dir, struct dentry *entry, umode_t mode,
		      dev_t rdev)
{
	printk(KERN_INFO "%s\n", __PRETTY_FUNCTION__);
	return proxyfs_create_object(dir, entry, mode, rdev, NULL, NULL);
}

static int proxyfs_rename(struct inode *olddir, struct dentry *oldent,
		       struct inode *newdir, struct dentry *newent)
{
	struct dentry *b_olddir;
	struct dentry *b_oldent;
	struct dentry *b_newdir;
	struct dentry *b_newent;
	int err;

	struct dentry *trap = NULL;
	struct inode *inode = d_inode(newent);

	b_olddir = proxyfs_get_b_dentry_resolved(olddir);
	err = PTR_ERR(b_olddir);
	if(IS_ERR(b_olddir)) {
		return err;
	}
	b_oldent = proxyfs_get_b_dentry_resolved(d_inode(oldent));
	err = PTR_ERR(b_oldent);
	if(IS_ERR(b_oldent)) {
		return err;
	}
	b_newdir = proxyfs_get_b_dentry_resolved(newdir);
	err = PTR_ERR(b_newdir);
	if(IS_ERR(b_newdir)) {
		return err;
	}
	b_newent = proxyfs_resolve_dentry(newent);
	if(!b_newent)
		return -EIO;

	trap = lock_rename(b_olddir, b_newdir);
	if(trap == b_oldent) {
		unlock_rename(b_olddir, b_newdir);
		dput(b_newent);
		return -EINVAL;
	}
	if(trap == b_newent) {
		unlock_rename(b_olddir, b_newdir);
		dput(b_newent);
		return -ENOTEMPTY;
	}
	err = vfs_rename(d_inode(b_olddir), b_oldent, d_inode(b_newdir), b_newent, NULL, 0);
	if(err) {
		unlock_rename(b_olddir, b_newdir);
		dput(b_newent);
		return err;
	}
	if(inode)
		fsstack_copy_attr_all(inode, d_inode(b_newent));
	fsstack_copy_attr_all(newdir, d_inode(b_newdir));
	if (newdir != olddir)
		fsstack_copy_attr_all(olddir, d_inode(b_olddir));

	unlock_rename(b_olddir, b_newdir);
	dput(b_newent);
	return err;
}

static int proxyfs_readlink(struct dentry *entry, char __user *buffer, int buflen)
{
	struct inode *inode;
	struct path b_path;
	struct dentry* b_dentry;
	struct inode *b_inode;
	int err;

	printk(KERN_INFO "%s\n", __PRETTY_FUNCTION__);

	inode = d_inode(entry);
	b_dentry = proxyfs_get_b_dentry_resolved(inode);
	err = PTR_ERR(b_dentry);
	if(IS_ERR(b_dentry)) {
		return err;
	}

	b_inode = d_inode(b_dentry);
	if(!b_inode)
		return -ENOENT;

	if (!b_inode->i_op->readlink)
		return -EINVAL;

	b_path.mnt = proxyfs_get_b_mount(inode);
	b_path.dentry = b_dentry;
	touch_atime(&b_path);

	return b_inode->i_op->readlink(b_dentry, buffer, buflen);
}

/*
static void *proxyfs_follow_link(struct dentry *dentry, struct nameidata *nd)
{

}

static void proxyfs_put_link(struct dentry *dentry, struct nameidata *nd, void *c)
{

}
*/

static int proxyfs_permission(struct inode *inode, int mask)
{
	printk(KERN_INFO "%s\n", __PRETTY_FUNCTION__);

	/* Permission always granted for ProxyFS */

	return 0;
}

static int proxyfs_setattr(struct dentry *entry, struct iattr *attr)
{
	struct inode *inode;
	struct dentry * b_dentry;
	struct vfsmount* b_mnt;
	int err;

	printk(KERN_INFO "%s\n", __PRETTY_FUNCTION__);

	inode = d_inode(entry);

	b_dentry = proxyfs_get_b_dentry_resolved(inode);
	err = PTR_ERR(b_dentry);
	if(IS_ERR(b_dentry)) {
		return err;
	}
	
	b_mnt = proxyfs_get_b_mount(inode);
	err = mnt_want_write(b_mnt);
	if(err)
		return err;

	mutex_lock(&b_dentry->d_inode->i_mutex);
	err = notify_change(b_dentry, attr, NULL);
	mutex_unlock(&b_dentry->d_inode->i_mutex);

	mnt_drop_write(b_mnt);

	return err;
}

static int proxyfs_getattr(struct vfsmount *mnt, struct dentry *entry,
			struct kstat *stat)
{
	struct path b_path;
	struct dentry* b_dentry;
	struct inode* b_inode;
	struct inode* inode = entry->d_inode;

	printk(KERN_INFO "%s\n", __PRETTY_FUNCTION__);

	b_dentry = proxyfs_get_b_dentry_resolved(inode);
	if(IS_ERR(b_dentry))
		return PTR_ERR(b_dentry);

	b_inode = d_inode(b_dentry);
	if(!b_inode)
		return -ENOENT;

	b_path.mnt = proxyfs_get_b_mount(inode);
	b_path.dentry = b_dentry;
	
	return vfs_getattr(&b_path, stat);
}

static int proxyfs_setxattr(struct dentry *entry, const char *name,
			 const void *value, size_t size, int flags)
{
	struct inode *inode;
	struct dentry * b_dentry;
	struct vfsmount* b_mnt;
	int err;

	printk(KERN_INFO "%s\n", __PRETTY_FUNCTION__);

	inode = d_inode(entry);

	b_dentry = proxyfs_get_b_dentry_resolved(inode);
	err = PTR_ERR(b_dentry);
	if(IS_ERR(b_dentry)) {
		return err;
	}
	
	b_mnt = proxyfs_get_b_mount(inode);
	err = mnt_want_write(b_mnt);
	if(err)
		return err;

	err = vfs_setxattr(b_dentry, name, value, size, flags);
	mnt_drop_write(b_mnt);

	return err;
}

static ssize_t proxyfs_getxattr(struct dentry *entry, const char *name,
			     void *value, size_t size)
{
	struct dentry* b_dentry;
	struct inode* b_inode;
	struct inode* inode = entry->d_inode;

	printk(KERN_INFO "%s\n", __PRETTY_FUNCTION__);

	b_dentry = proxyfs_get_b_dentry_resolved(inode);
	if(IS_ERR(b_dentry))
		return PTR_ERR(b_dentry);

	b_inode = d_inode(b_dentry);
	if(!b_inode)
		return -ENOENT;
	
	return vfs_getxattr(b_dentry, name, value, size);
}

static ssize_t proxyfs_listxattr(struct dentry *entry, char *list, size_t size)
{
	struct dentry* b_dentry;
	struct inode* b_inode;
	struct inode* inode = entry->d_inode;

	printk(KERN_INFO "%s\n", __PRETTY_FUNCTION__);

	b_dentry = proxyfs_get_b_dentry_resolved(inode);
	if(IS_ERR(b_dentry))
		return PTR_ERR(b_dentry);

	b_inode = d_inode(b_dentry);
	if(!b_inode)
		return -ENOENT;

	return vfs_listxattr(b_dentry, list, size);
}

static int proxyfs_removexattr(struct dentry *entry, const char *name)
{
	struct inode *inode;
	struct dentry * b_dentry;
	struct vfsmount* b_mnt;
	int err;

	printk(KERN_INFO "%s\n", __PRETTY_FUNCTION__);

	inode = d_inode(entry);

	b_dentry = proxyfs_get_b_dentry_resolved(inode);
	err = PTR_ERR(b_dentry);
	if(IS_ERR(b_dentry)) {
		return err;
	}
	
	b_mnt = proxyfs_get_b_mount(inode);
	err = mnt_want_write(b_mnt);
	if(err)
		return err;

	err = vfs_removexattr(b_dentry, name);
	mnt_drop_write(b_mnt);

	return err;
}

static const struct inode_operations proxyfs_inode_operations = {
	.create		= proxyfs_create,
	.lookup		= proxyfs_lookup,
	.link		= proxyfs_link,
	.unlink		= proxyfs_unlink,
	.symlink	= proxyfs_symlink,
	.mkdir		= proxyfs_mkdir,
	.rmdir		= proxyfs_rmdir,
	.mknod		= proxyfs_mknod,
	.rename		= proxyfs_rename,
	.readlink	= proxyfs_readlink,
	//.follow_link	= proxyfs_follow_link,
	//.put_link	= proxyfs_put_link,
	.permission	= proxyfs_permission,
	.setattr	= proxyfs_setattr,
	.getattr	= proxyfs_getattr,
	.setxattr	= proxyfs_setxattr,
	.getxattr	= proxyfs_getxattr,
	.listxattr	= proxyfs_listxattr,
	.removexattr	= proxyfs_removexattr,
};

static struct inode *proxyfs_alloc_inode(struct super_block *sb)
{
	struct inode *inode;
	struct proxyfs_inode *pi;

	inode = kmem_cache_alloc(proxyfs_inode_cachep, GFP_KERNEL);
	if (!inode)
		return NULL;

	printk(KERN_INFO "%s %016llX\n", __PRETTY_FUNCTION__, (unsigned long long)inode);

	pi = get_proxyfs_inode(inode);
	pi->b_dentry = NULL;

	return inode;
}

static void proxyfs_i_callback(struct rcu_head *head)
{
	struct inode *inode = container_of(head, struct inode, i_rcu);

	if(proxyfs_resolved_inode(inode))
		dput(proxyfs_get_b_dentry(inode));

	kmem_cache_free(proxyfs_inode_cachep, inode);
}

static void proxyfs_destroy_inode(struct inode *inode)
{
	dump_stack();
	printk(KERN_INFO "%s %016llX\n", __PRETTY_FUNCTION__, (unsigned long long)inode);
	call_rcu(&inode->i_rcu, proxyfs_i_callback);
}

static void proxyfs_init_inode(struct inode *inode, struct inode *dir, mode_t mode)
{
	printk(KERN_INFO "%s\n", __PRETTY_FUNCTION__);
	
	inode->i_ino = get_next_ino();
	inode_init_owner(inode, dir, mode);
	//inode->i_mapping->a_ops = &ramfs_aops;
	inode->i_atime = inode->i_mtime = inode->i_ctime = CURRENT_TIME;
	if(special_file(inode->i_mode)) {
		init_special_inode(inode, inode->i_mode, inode->i_rdev);
	} else {
		inode->i_op = &proxyfs_inode_operations;
		//inode->i_fop = &proxy_file_operations;
	}
}

static int proxyfs_inode_eq(struct inode *inode, void *data)
{
	struct dentry* entry = (struct dentry*)data;
	struct proxyfs_inode *pi = get_proxyfs_inode(inode);

	if(!entry) {
		return 0;
	}

	if(pi->p_dentry->d_name.len != entry->d_name.len)
		return 0;

	return (memcmp(pi->p_dentry->d_name.name, entry->d_name.name, pi->p_dentry->d_name.len) == 0);
}

static int proxyfs_inode_set(struct inode *inode, void *data)
{
	struct dentry* entry = (struct dentry*)data;
	get_proxyfs_inode(inode)->p_dentry = entry;
	return 0;
}

static struct inode* proxyfs_iget(struct super_block *sb, struct inode *dir, struct dentry *entry, mode_t mode)
{
	struct inode *inode;
	unsigned long dhash;

	printk(KERN_INFO "%s\n", __PRETTY_FUNCTION__);

	if(entry)
		dhash = full_name_hash(entry->d_name.name, entry->d_name.len);
	else
		dhash = full_name_hash("/", 1);
	
	inode = iget5_locked(sb, dhash, proxyfs_inode_eq, proxyfs_inode_set, entry);
	if (!inode)
		return NULL;

	if ((inode->i_state & I_NEW)) {
		inode->i_flags |= S_NOATIME;
		proxyfs_init_inode(inode, dir, mode);
		unlock_new_inode(inode);
	}

	return inode;
}

enum {
	OPT_MODE,
	OPT_BACKEND,
	OPT_ERR
};

static const match_table_t tokens = {
	{OPT_MODE, "mode=%o"},
	{OPT_BACKEND, "backend=%s"},
	{OPT_ERR, NULL}
};

static int proxyfs_parse_options(char *data, struct proxyfs_mount_opts *opts)
{
	substring_t args[MAX_OPT_ARGS];
	int option;
	int token;
	int option_backend = 0;
	char *p;
	printk(KERN_INFO "%s\n", __PRETTY_FUNCTION__);

	opts->mode = PROXYFS_DEFAULT_MODE;

	while ((p = strsep(&data, ",")) != NULL) {
		if (!*p)
			continue;

		token = match_token(p, tokens, args);
		switch (token) {
		case OPT_MODE:
			if (match_octal(&args[0], &option))
				return -EINVAL;
			opts->mode = option & S_IALLUGO;
			break;
		case OPT_BACKEND:
			opts->backend = match_strdup(&args[0]);
			if(!opts->backend)
				return -EINVAL;
			option_backend = 1;
			break;
		}
	}

	/* Backend option is required */
	if(!option_backend) {
		printk(KERN_ERR "ProxyFS: backend option is required\n");
		return -EINVAL;
	}

	return 0;
}

static const struct super_operations proxyfs_ops = {
	.alloc_inode    = proxyfs_alloc_inode,
	.destroy_inode  = proxyfs_destroy_inode,
	.statfs		= simple_statfs,
	.drop_inode	= generic_delete_inode,
	.show_options	= generic_show_options,
};

static int proxyfs_fill_super(struct super_block *sb, void *data, int silent)
{
	struct inode *inode;
	struct proxyfs_inode *pi;
	struct proxyfs_fs_info *fsi;
	int err;
	struct path path;
	printk(KERN_INFO "%s\n", __PRETTY_FUNCTION__);

	save_mount_options(sb, data);

	fsi = kzalloc(sizeof(struct proxyfs_fs_info), GFP_KERNEL);
	sb->s_fs_info = fsi;
	if (!fsi)
		return -ENOMEM;

	err = proxyfs_parse_options(data, &fsi->mount_opts);
	if (err)
		return err;

	sb->s_maxbytes		= MAX_LFS_FILESIZE;
	sb->s_blocksize		= PAGE_CACHE_SIZE;
	sb->s_blocksize_bits	= PAGE_CACHE_SHIFT;
	sb->s_magic		= PROXYFS_MAGIC;
	sb->s_op		= &proxyfs_ops;
	sb->s_time_gran		= 1;

	err = kern_path(fsi->mount_opts.backend, LOOKUP_FOLLOW | LOOKUP_DIRECTORY, &path);
	if(err)
		return err;
	
	if(!S_ISDIR(path.dentry->d_inode->i_mode)) {
		printk(KERN_ERR "ProxyFS: backend path: %s must be a directory\n", fsi->mount_opts.backend);
		path_put(&path);
		return -EINVAL;
	}

	fsi->b_mount = path.mnt;

	inode = proxyfs_iget(sb, NULL, NULL, S_IFDIR | fsi->mount_opts.mode);
	sb->s_root = d_make_root(inode);
	if (!sb->s_root) {
		path_put(&path);
		return -ENOMEM;
	}

	pi = get_proxyfs_inode(inode);
	pi->p_dentry = sb->s_root;
	pi->b_dentry = path.dentry;
	
	path_put(&path);

	return 0;
}

static struct dentry *proxyfs_mount(struct file_system_type *fs_type,
		       int flags, const char *dev_name,
		       void *raw_data)
{
	printk(KERN_INFO "%s", __PRETTY_FUNCTION__);
	return mount_nodev(fs_type, flags, raw_data, proxyfs_fill_super);
}

static void proxyfs_kill_sb(struct super_block *sb)
{
	printk(KERN_INFO "%s", __PRETTY_FUNCTION__);
	kfree(((struct proxyfs_fs_info *)sb->s_fs_info)->mount_opts.backend);
	kfree(sb->s_fs_info);
	kill_anon_super(sb);
}

static struct file_system_type proxyfs_fs_type = {
	.owner		= THIS_MODULE,
	.name		= "proxyfs",
	.fs_flags	= FS_USERNS_MOUNT,
	.mount		= proxyfs_mount,
	.kill_sb	= proxyfs_kill_sb,
};
MODULE_ALIAS_FS("proxyfs");

static void proxyfs_inode_init_once(void *foo)
{
	struct inode *inode = foo;

	inode_init_once(inode);
}

static int __init proxyfs_init(void)
{
	static unsigned long once;

	if (test_and_set_bit(0, &once))
		return 0;
	
	printk(KERN_INFO "ProxyFS init\n");

	proxyfs_inode_cachep = kmem_cache_create("proxyfs_inode",
		  					  sizeof(struct proxyfs_inode),
					    	  0, SLAB_HWCACHE_ALIGN,
					    	  proxyfs_inode_init_once);

	return register_filesystem(&proxyfs_fs_type);
	return 0;
}

static void __exit proxyfs_exit(void)
{
	printk(KERN_DEBUG "ProxyFS exit\n");
	unregister_filesystem(&proxyfs_fs_type);

	/*
	 * Make sure all delayed rcu free inodes are flushed before we
	 * destroy cache.
	 */
	rcu_barrier();
	kmem_cache_destroy(proxyfs_inode_cachep);
}

module_init(proxyfs_init);
module_exit(proxyfs_exit);

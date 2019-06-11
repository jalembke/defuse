/*
  DEFUSE File System
  Copyright (C) 2017  James Lembke <jalembke@gmail.com>

  This program can be distributed under the terms of the GNU GPL.
  See the file COPYING.
*/

#include "defuse.h"

#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/namei.h>
#include <linux/pagemap.h>
#include <linux/time.h>
#include <linux/string.h>
#include <linux/parser.h>
#include <linux/slab.h>
#include <linux/mount.h>
#include <asm/uaccess.h>
#include <linux/xattr.h>

MODULE_AUTHOR("James Lembke <jalembke@gmail.com>");
MODULE_DESCRIPTION("Proxy File System");
MODULE_LICENSE("GPL");

#define DEFUSE_DEFAULT_MODE	0755
#define DEFUSE_MAGIC			0x50525859	// PRXY

static struct kmem_cache *defuse_inode_cachep;

static struct inode* defuse_iget(struct super_block *sb, struct inode *dir, struct dentry *entry, mode_t mode);

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
struct dentry* defuse_resolve_dentry(struct dentry* entry)
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
	if(!defuse_resolved_inode(p_parent->d_inode)) {
		b_parent = defuse_resolve_dentry(p_parent);
		if(!b_parent) {
			dput(p_parent);
			return NULL;
		}
		get_defuse_inode(p_parent->d_inode)->b_dentry = b_parent;
	} else {
		b_parent = defuse_get_b_dentry(p_parent->d_inode);
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

static inline int defuse_do_create(struct inode *dir, struct dentry *entry, umode_t mode, 
	dev_t dev, const char* link, struct dentry *hardlink)
{
	struct inode *inode;
	struct dentry *b_dir;
	struct dentry *b_dentry;
	struct dentry *b_hardlink;
	int err;
	
	PRINTFN;

	b_dir = defuse_get_b_dentry_resolved(dir);
	err = PTR_ERR(b_dir);
	if(IS_ERR(b_dir)) {
		return err;
	}
	if(hardlink) {
		b_hardlink = defuse_get_b_dentry_resolved(d_inode(hardlink));
		err = PTR_ERR(b_hardlink);
		if(IS_ERR(b_hardlink)) {
			return err;
		}
	}
	b_dentry = defuse_resolve_dentry(entry);
	if(!b_dentry)
		return -EIO;
	if(d_inode(b_dentry)) {
		dput(b_dentry);
		return -ESTALE;
	}

	inode = defuse_iget(dir->i_sb, dir, entry, mode);
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
	get_defuse_inode(inode)->b_dentry = b_dentry;
	return 0;
}

static inline int defuse_create_object(struct inode *dir, struct dentry *entry, umode_t mode, 
	dev_t dev, const char* link, struct dentry* hardlink)
{
	struct vfsmount* b_mnt;
	int err;
	
	PRINTFN;

	b_mnt = defuse_get_b_mount(dir);
	err = mnt_want_write(b_mnt);
	if(err) {
		return err;
	}
	err = defuse_do_create(dir, entry, mode, dev, link, hardlink);
	mnt_drop_write(b_mnt);
	
	return err;
}

static inline int defuse_do_delete(struct inode *dir, struct dentry *entry)
{
	struct inode *inode;
	struct dentry *b_dir;
	struct dentry * b_dentry;
	int err;

	PRINTFN;

	b_dir = defuse_get_b_dentry_resolved(dir);
	err = PTR_ERR(b_dir);
	if(IS_ERR(b_dir)) {
		return err;
	}

	inode = d_inode(entry);
	b_dentry = defuse_get_b_dentry_resolved(inode);
	err = PTR_ERR(b_dentry);
	if(IS_ERR(b_dentry)) {
		return err;
	}

	mutex_lock_nested(&b_dir->d_inode->i_mutex, I_MUTEX_PARENT);
	if(S_ISDIR(b_dentry->d_inode->i_mode)) {
		err = vfs_rmdir(d_inode(b_dir), b_dentry);
	} else {
		err = vfs_unlink(d_inode(b_dir), b_dentry, NULL);
	}
	mutex_unlock(&b_dir->d_inode->i_mutex);

	return err;
}

static inline int defuse_delete_object(struct inode *dir, struct dentry *entry)
{
	struct vfsmount* b_mnt;
	int err;

	b_mnt = defuse_get_b_mount(dir);
	err = mnt_want_write(b_mnt);
	if(err) {
		return err;
	}
	err = defuse_do_delete(dir, entry);
	mnt_drop_write(b_mnt);
	
	return err;
}

static int defuse_create(struct inode *dir, struct dentry *entry, umode_t mode,
			bool want_excl)
{
	PRINTFN;
	return defuse_create_object(dir, entry, (mode & 07777) | S_IFREG, 0, NULL, NULL);
}

struct dentry *defuse_lookup(struct inode *dir, struct dentry *entry, unsigned int flags)
{
	struct inode *inode = NULL;
	struct dentry *b_dentry = NULL;
	mode_t base_mode = S_IFREG;
	
	PRINTFN;

	if(flags & LOOKUP_DIRECTORY)
		base_mode = S_IFDIR;
	
	if(flags & LOOKUP_CREATE) {
		b_dentry = defuse_resolve_dentry(entry);
		if(!b_dentry)
			return ERR_PTR(-EIO);
		if(b_dentry->d_inode) {
			inode = defuse_iget(dir->i_sb, dir, entry, b_dentry->d_inode->i_mode);
			if(!inode) {
				dput(b_dentry);
				return ERR_PTR(-ENOSPC);
			}
			get_defuse_inode(inode)->b_dentry = b_dentry;
		} else {
			dput(b_dentry);
		}
	} else {
		inode = defuse_iget(dir->i_sb, dir, entry, base_mode | DEFUSE_DEFAULT_MODE);
		if(!inode)
			return ERR_PTR(-ENOSPC);
	}

	return d_splice_alias(inode, entry);
}

static int defuse_link(struct dentry *entry, struct inode *newdir,
		     struct dentry *newent)
{
	PRINTFN;
	return defuse_create_object(newdir, newent, entry->d_inode->i_mode, 0, NULL, entry);
}

static int defuse_unlink(struct inode *dir, struct dentry *entry)
{
	PRINTFN;
	return defuse_delete_object(dir, entry);
}

static int defuse_symlink(struct inode *dir, struct dentry *entry,
			const char *link)
{
	PRINTFN;
	return defuse_create_object(dir, entry, S_IFLNK, 0, link, NULL);
}

static int defuse_mkdir(struct inode *dir, struct dentry *entry, umode_t mode)
{
	PRINTFN;
	return defuse_create_object(dir, entry, (mode & 07777) | S_IFDIR, 0, NULL, NULL);
}

static int defuse_rmdir(struct inode *dir, struct dentry *entry)
{
	PRINTFN;
	return defuse_delete_object(dir, entry);
}

static int defuse_mknod(struct inode *dir, struct dentry *entry, umode_t mode,
		      dev_t rdev)
{
	PRINTFN;
	return defuse_create_object(dir, entry, mode, rdev, NULL, NULL);
}

static int defuse_rename(struct inode *olddir, struct dentry *oldent,
		       struct inode *newdir, struct dentry *newent)
{
	struct dentry *b_olddir;
	struct dentry *b_oldent;
	struct dentry *b_newdir;
	struct dentry *b_newent;
	int err;

	struct dentry *trap = NULL;
	
	PRINTFN;

	b_olddir = defuse_get_b_dentry_resolved(olddir);
	err = PTR_ERR(b_olddir);
	if(IS_ERR(b_olddir)) {
		return err;
	}
	b_oldent = defuse_get_b_dentry_resolved(d_inode(oldent));
	err = PTR_ERR(b_oldent);
	if(IS_ERR(b_oldent)) {
		return err;
	}
	b_newdir = defuse_get_b_dentry_resolved(newdir);
	err = PTR_ERR(b_newdir);
	if(IS_ERR(b_newdir)) {
		return err;
	}
	b_newent = defuse_resolve_dentry(newent);
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

	unlock_rename(b_olddir, b_newdir);
	dput(b_newent);
	return err;
}

static int defuse_readlink(struct dentry *entry, char __user *buffer, int buflen)
{
	struct inode *inode;
	struct path b_path;
	struct dentry* b_dentry;
	struct inode *b_inode;
	int err;

	PRINTFN;

	inode = d_inode(entry);
	b_dentry = defuse_get_b_dentry_resolved(inode);
	err = PTR_ERR(b_dentry);
	if(IS_ERR(b_dentry)) {
		return err;
	}

	b_inode = d_inode(b_dentry);
	if(!b_inode)
		return -ENOENT;

	if (!b_inode->i_op->readlink)
		return -EINVAL;

	b_path.mnt = defuse_get_b_mount(inode);
	b_path.dentry = b_dentry;
	touch_atime(&b_path);

	return b_inode->i_op->readlink(b_dentry, buffer, buflen);
}

/*
static void *defuse_follow_link(struct dentry *dentry, struct nameidata *nd)
{

}

static void defuse_put_link(struct dentry *dentry, struct nameidata *nd, void *c)
{

}
*/

static int defuse_permission(struct inode *inode, int mask)
{
	PRINTFN;

	/* Permission always granted for defuse */

	return 0;
}

static int defuse_setattr(struct dentry *entry, struct iattr *attr)
{
	struct inode *inode;
	struct dentry * b_dentry;
	struct vfsmount* b_mnt;
	int err;

	PRINTFN;

	inode = d_inode(entry);

	b_dentry = defuse_get_b_dentry_resolved(inode);
	err = PTR_ERR(b_dentry);
	if(IS_ERR(b_dentry)) {
		return err;
	}
	
	b_mnt = defuse_get_b_mount(inode);
	err = mnt_want_write(b_mnt);
	if(err)
		return err;

	mutex_lock(&b_dentry->d_inode->i_mutex);
	err = notify_change(b_dentry, attr, NULL);
	mutex_unlock(&b_dentry->d_inode->i_mutex);

	mnt_drop_write(b_mnt);

	return err;
}

static int defuse_getattr(struct vfsmount *mnt, struct dentry *entry,
			struct kstat *stat)
{
	struct path b_path;
	struct dentry* b_dentry;
	struct inode* b_inode;
	struct inode* inode = entry->d_inode;

	PRINTFN;

	b_dentry = defuse_get_b_dentry_resolved(inode);
	if(IS_ERR(b_dentry))
		return PTR_ERR(b_dentry);

	b_inode = d_inode(b_dentry);
	if(!b_inode) {
		printk(KERN_INFO "HERE\n");
		return -ENOENT;
	}

	b_path.mnt = defuse_get_b_mount(inode);
	b_path.dentry = b_dentry;
	
	return vfs_getattr(&b_path, stat);
}

static int defuse_setxattr(struct dentry *entry, const char *name,
			 const void *value, size_t size, int flags)
{
	struct inode *inode;
	struct dentry * b_dentry;
	struct vfsmount* b_mnt;
	int err;

	PRINTFN;

	inode = d_inode(entry);

	b_dentry = defuse_get_b_dentry_resolved(inode);
	err = PTR_ERR(b_dentry);
	if(IS_ERR(b_dentry)) {
		return err;
	}
	
	b_mnt = defuse_get_b_mount(inode);
	err = mnt_want_write(b_mnt);
	if(err)
		return err;

	err = vfs_setxattr(b_dentry, name, value, size, flags);
	mnt_drop_write(b_mnt);

	return err;
}

static ssize_t defuse_getxattr(struct dentry *entry, const char *name,
			     void *value, size_t size)
{
	struct dentry* b_dentry;
	struct inode* b_inode;
	struct inode* inode = entry->d_inode;

	PRINTFN;

	b_dentry = defuse_get_b_dentry_resolved(inode);
	if(IS_ERR(b_dentry))
		return PTR_ERR(b_dentry);

	b_inode = d_inode(b_dentry);
	if(!b_inode)
		return -ENOENT;
	
	return vfs_getxattr(b_dentry, name, value, size);
}

static ssize_t defuse_listxattr(struct dentry *entry, char *list, size_t size)
{
	struct dentry* b_dentry;
	struct inode* b_inode;
	struct inode* inode = entry->d_inode;

	PRINTFN;

	b_dentry = defuse_get_b_dentry_resolved(inode);
	if(IS_ERR(b_dentry))
		return PTR_ERR(b_dentry);

	b_inode = d_inode(b_dentry);
	if(!b_inode)
		return -ENOENT;

	return vfs_listxattr(b_dentry, list, size);
}

static int defuse_removexattr(struct dentry *entry, const char *name)
{
	struct inode *inode;
	struct dentry * b_dentry;
	struct vfsmount* b_mnt;
	int err;

	PRINTFN;

	inode = d_inode(entry);

	b_dentry = defuse_get_b_dentry_resolved(inode);
	err = PTR_ERR(b_dentry);
	if(IS_ERR(b_dentry)) {
		return err;
	}
	
	b_mnt = defuse_get_b_mount(inode);
	err = mnt_want_write(b_mnt);
	if(err)
		return err;

	err = vfs_removexattr(b_dentry, name);
	mnt_drop_write(b_mnt);

	return err;
}

static const struct inode_operations defuse_inode_operations = {
	.create		= defuse_create,
	.lookup		= defuse_lookup,
	.link		= defuse_link,
	.unlink		= defuse_unlink,
	.symlink	= defuse_symlink,
	.mkdir		= defuse_mkdir,
	.rmdir		= defuse_rmdir,
	.mknod		= defuse_mknod,
	.rename		= defuse_rename,
	.readlink	= defuse_readlink,
	//.follow_link	= defuse_follow_link,
	//.put_link	= defuse_put_link,
	.permission	= defuse_permission,
	.setattr	= defuse_setattr,
	.getattr	= defuse_getattr,
	.setxattr	= defuse_setxattr,
	.getxattr	= defuse_getxattr,
	.listxattr	= defuse_listxattr,
	.removexattr	= defuse_removexattr,
};

static struct inode *defuse_alloc_inode(struct super_block *sb)
{
	struct inode *inode;
	struct defuse_inode *pi;
	
	PRINTFN;

	inode = kmem_cache_alloc(defuse_inode_cachep, GFP_KERNEL);
	if (!inode)
		return NULL;

	pi = get_defuse_inode(inode);
	pi->b_dentry = NULL;

	return inode;
}

static void defuse_i_callback(struct rcu_head *head)
{
	struct inode *inode = container_of(head, struct inode, i_rcu);

	if(defuse_resolved_inode(inode))
		dput(defuse_get_b_dentry(inode));

	kmem_cache_free(defuse_inode_cachep, inode);
}

static void defuse_destroy_inode(struct inode *inode)
{
	PRINTFN;
	call_rcu(&inode->i_rcu, defuse_i_callback);
}

static void defuse_init_inode(struct inode *inode, struct inode *dir, mode_t mode)
{
	PRINTFN;
	
	inode->i_ino = get_next_ino();
	inode_init_owner(inode, dir, mode);
	inode->i_atime = inode->i_mtime = inode->i_ctime = CURRENT_TIME;
	if(special_file(inode->i_mode)) {
		init_special_inode(inode, inode->i_mode, inode->i_rdev);
	} else {
		inode->i_op = &defuse_inode_operations;
		inode->i_fop = &proxy_file_operations;
	}
}

static int defuse_inode_eq(struct inode *inode, void *data)
{
	struct dentry* entry = (struct dentry*)data;
	struct defuse_inode *pi = get_defuse_inode(inode);

	if(!entry) {
		return 0;
	}

	if(pi->p_dentry->d_name.len != entry->d_name.len)
		return 0;

	return (memcmp(pi->p_dentry->d_name.name, entry->d_name.name, pi->p_dentry->d_name.len) == 0);
}

static int defuse_inode_set(struct inode *inode, void *data)
{
	struct dentry* entry = (struct dentry*)data;
	get_defuse_inode(inode)->p_dentry = entry;
	return 0;
}

static struct inode* defuse_iget(struct super_block *sb, struct inode *dir, struct dentry *entry, mode_t mode)
{
	struct inode *inode;
	unsigned long dhash;

	PRINTFN;

	if(entry)
		dhash = full_name_hash(entry->d_name.name, entry->d_name.len);
	else
		dhash = full_name_hash("/", 1);
	
	inode = iget5_locked(sb, dhash, defuse_inode_eq, defuse_inode_set, entry);
	if (!inode)
		return NULL;

	if ((inode->i_state & I_NEW)) {
		inode->i_flags |= S_NOATIME;
		defuse_init_inode(inode, dir, mode);
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

static int defuse_parse_options(char *data, struct defuse_mount_opts *opts)
{
	substring_t args[MAX_OPT_ARGS];
	int option;
	int token;
	int option_backend = 0;
	char *p;
	
	PRINTFN;

	opts->mode = DEFUSE_DEFAULT_MODE;

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
		printk(KERN_ERR "defuse: backend option is required\n");
		return -EINVAL;
	}

	return 0;
}

static const struct super_operations defuse_ops = {
	.alloc_inode    = defuse_alloc_inode,
	.destroy_inode  = defuse_destroy_inode,
	.statfs		= simple_statfs,
	.drop_inode	= generic_delete_inode,
	.show_options	= generic_show_options,
};

static int defuse_fill_super(struct super_block *sb, void *data, int silent)
{
	struct inode *inode;
	struct defuse_inode *pi;
	struct defuse_fs_info *fsi;
	int err;
	struct path path;

	PRINTFN;

	save_mount_options(sb, data);

	fsi = kzalloc(sizeof(struct defuse_fs_info), GFP_KERNEL);
	sb->s_fs_info = fsi;
	if (!fsi)
		return -ENOMEM;

	err = defuse_parse_options(data, &fsi->mount_opts);
	if (err)
		return err;

	sb->s_maxbytes		= MAX_LFS_FILESIZE;
	sb->s_blocksize		= PAGE_CACHE_SIZE;
	sb->s_blocksize_bits	= PAGE_CACHE_SHIFT;
	sb->s_magic		= DEFUSE_MAGIC;
	sb->s_op		= &defuse_ops;
	sb->s_time_gran		= 1;

	err = kern_path(fsi->mount_opts.backend, LOOKUP_FOLLOW | LOOKUP_DIRECTORY, &path);
	if(err)
		return err;
	
	if(!S_ISDIR(path.dentry->d_inode->i_mode)) {
		printk(KERN_ERR "defuse: backend path: %s must be a directory\n", fsi->mount_opts.backend);
		path_put(&path);
		return -EINVAL;
	}

	fsi->b_mount = path.mnt;

	inode = defuse_iget(sb, NULL, NULL, S_IFDIR | fsi->mount_opts.mode);
	sb->s_root = d_make_root(inode);
	if (!sb->s_root) {
		path_put(&path);
		return -ENOMEM;
	}

	pi = get_defuse_inode(inode);
	pi->p_dentry = sb->s_root;
	pi->b_dentry = path.dentry;
	
	path_put(&path);

	return 0;
}

static struct dentry *defuse_mount(struct file_system_type *fs_type,
		       int flags, const char *dev_name,
		       void *raw_data)
{
	PRINTFN;
	return mount_nodev(fs_type, flags, raw_data, defuse_fill_super);
}

static void defuse_kill_sb(struct super_block *sb)
{
	PRINTFN;
	kfree(((struct defuse_fs_info *)sb->s_fs_info)->mount_opts.backend);
	kfree(sb->s_fs_info);
	kill_anon_super(sb);
}

static struct file_system_type defuse_fs_type = {
	.owner		= THIS_MODULE,
	.name		= "defuse",
	.fs_flags	= FS_USERNS_MOUNT,
	.mount		= defuse_mount,
	.kill_sb	= defuse_kill_sb,
};
MODULE_ALIAS_FS("defuse");

static void defuse_inode_init_once(void *foo)
{
	struct inode *inode = foo;

	inode_init_once(inode);
}

static int __init defuse_init(void)
{
	static unsigned long once;

	if (test_and_set_bit(0, &once))
		return 0;
	
	printk(KERN_INFO "defuse init\n");

	defuse_inode_cachep = kmem_cache_create("defuse_inode",
		  					  sizeof(struct defuse_inode),
					    	  0, SLAB_HWCACHE_ALIGN,
					    	  defuse_inode_init_once);

	return register_filesystem(&defuse_fs_type);
	return 0;
}

static void __exit defuse_exit(void)
{
	printk(KERN_DEBUG "defuse exit\n");
	unregister_filesystem(&defuse_fs_type);

	/*
	 * Make sure all delayed rcu free inodes are flushed before we
	 * destroy cache.
	 */
	rcu_barrier();
	kmem_cache_destroy(defuse_inode_cachep);
}

module_init(defuse_init);
module_exit(defuse_exit);

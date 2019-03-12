/*
  BOPFS File System
  Copyright (C) 2019  James Lembke <jalembke@gmail.com>

  This program can be distributed under the terms of the GNU GPL.
  See the file COPYING.
*/

#include "bopfs.h"

#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/seq_file.h>
#include <linux/namei.h>
#include <linux/time.h>
#include <linux/string.h>
#include <linux/parser.h>
#include <linux/slab.h>
#include <linux/mount.h>

MODULE_AUTHOR("James Lembke <jalembke@gmail.com>");
MODULE_DESCRIPTION("Bypassed Open File System");
MODULE_LICENSE("GPL");

#define BOPFS_DEFAULT_MODE	0755
#define BOPFS_MAGIC			0x50525859	// PRXY

static struct kmem_cache *bopfs_inode_cachep;

static struct inode* bopfs_iget(struct super_block *sb, struct inode *dir, struct dentry *entry, mode_t mode);

struct dentry *bopfs_lookup(struct inode *dir, struct dentry *entry, unsigned int flags)
{
	struct inode *inode = NULL;
	mode_t base_mode = S_IFDIR;
	
	PRINTFN;
	
	inode = bopfs_iget(dir->i_sb, dir, entry, base_mode | BOPFS_DEFAULT_MODE);
	if(!inode)
		return ERR_PTR(-ENOSPC);

	return d_splice_alias(inode, entry);
}

static int bopfs_permission(struct inode *inode, int mask)
{
	PRINTFN;

	/* Permission always granted for bopfs */

	return 0;
}

static const struct inode_operations bopfs_inode_operations = {
	.lookup		= bopfs_lookup,
	.permission	= bopfs_permission,
};

static const struct file_operations bopfs_file_operations = {
	.llseek     = default_llseek,
	.open       = generic_file_open,
};

static struct inode *bopfs_alloc_inode(struct super_block *sb)
{
	struct inode *inode;
	struct bopfs_inode *pi;
	
	PRINTFN;

	inode = kmem_cache_alloc(bopfs_inode_cachep, GFP_KERNEL);
	if (!inode)
		return NULL;

	pi = get_bopfs_inode(inode);
	pi->p_dentry = NULL;

	return inode;
}

static void bopfs_i_callback(struct rcu_head *head)
{
	struct inode *inode = container_of(head, struct inode, i_rcu);
	kmem_cache_free(bopfs_inode_cachep, inode);
}

static void bopfs_destroy_inode(struct inode *inode)
{
	PRINTFN;
	call_rcu(&inode->i_rcu, bopfs_i_callback);
}

static void bopfs_init_inode(struct inode *inode, struct inode *dir, mode_t mode)
{
	PRINTFN;
	inode->i_ino = get_next_ino();
	inode_init_owner(inode, dir, mode);
	inode->i_atime = inode->i_mtime = inode->i_ctime = current_time(inode);
	inode->i_op = &bopfs_inode_operations;
	inode->i_fop = &bopfs_file_operations;
}

static int bopfs_inode_eq(struct inode *inode, void *data)
{
	struct dentry* entry = (struct dentry*)data;
	struct bopfs_inode *pi = get_bopfs_inode(inode);

	if(!entry) {
		return 0;
	}

	if(pi->p_dentry->d_name.len != entry->d_name.len)
		return 0;

	return (memcmp(pi->p_dentry->d_name.name, entry->d_name.name, pi->p_dentry->d_name.len) == 0);
}

static int bopfs_inode_set(struct inode *inode, void *data)
{
	struct dentry* entry = (struct dentry*)data;
	get_bopfs_inode(inode)->p_dentry = entry;
	return 0;
}

static struct inode* bopfs_iget(struct super_block *sb, struct inode *dir, struct dentry *entry, mode_t mode)
{
	struct inode *inode;
	unsigned long dhash;

	PRINTFN;

	if(entry)
		dhash = full_name_hash(entry, entry->d_name.name, entry->d_name.len);
	else
		dhash = full_name_hash(entry, "/", 1);
	
	inode = iget5_locked(sb, dhash, bopfs_inode_eq, bopfs_inode_set, entry);
	if (!inode)
		return NULL;

	if ((inode->i_state & I_NEW)) {
		inode->i_flags |= S_NOATIME;
		bopfs_init_inode(inode, dir, mode);
		unlock_new_inode(inode);
	}

	return inode;
}

enum {
	OPT_MODE,
	OPT_BACKEND,
	OPT_USER_SPACE_LIBRARY,
	OPT_ERR
};

static const match_table_t tokens = {
	{OPT_MODE, "mode=%o"},
	{OPT_BACKEND, "backend=%s"},
	{OPT_USER_SPACE_LIBRARY, "library=%s"},
	{OPT_ERR, NULL}
};

static int bopfs_parse_options(char *data, struct bopfs_mount_opts *opts)
{
	substring_t args[MAX_OPT_ARGS];
	int option;
	int token;
	int option_backend = 0;
	int option_library = 0;
	char *p;
	
	PRINTFN;

	opts->mode = BOPFS_DEFAULT_MODE;

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
			opts->backend_path = match_strdup(&args[0]);
			if(!opts->backend_path)
				return -EINVAL;
			option_backend = 1;
			break;
		case OPT_USER_SPACE_LIBRARY:
			opts->user_library_path = match_strdup(&args[0]);
			if(!opts->user_library_path)
				return -EINVAL;
			option_library = 1;
			break;
		}
	}

	/* Backend option is required */
	if(!option_backend) {
		printk(KERN_ERR "bopfs: backend option is required\n");
		return -EINVAL;
	}
	/* User library is required */
	if(!option_library) {
		printk(KERN_ERR "bopfs: library option is required\n");
		return -EINVAL;
	}

	return 0;
}

static int bopfs_show_options(struct seq_file *m, struct dentry *root)
{
	struct bopfs_fs_info *fsi = root->d_sb->s_fs_info;
	seq_printf(m, ",backend=%s", fsi->mount_opts.backend_path);
	seq_printf(m, ",library=%s", fsi->mount_opts.user_library_path);
	return 0;
}

static const struct super_operations bopfs_ops = {
	.alloc_inode    = bopfs_alloc_inode,
	.destroy_inode  = bopfs_destroy_inode,
	.statfs		= simple_statfs,
	.drop_inode	= generic_delete_inode,
	.show_options	= bopfs_show_options,
};

static int bopfs_fill_super(struct super_block *sb, void *data, int silent)
{
	struct inode *inode;
	struct bopfs_inode *pi;
	struct bopfs_fs_info *fsi;
	int err;
	struct path path;

	PRINTFN;

	fsi = kzalloc(sizeof(struct bopfs_fs_info), GFP_KERNEL);
	sb->s_fs_info = fsi;
	if (!fsi)
		return -ENOMEM;

	err = bopfs_parse_options(data, &fsi->mount_opts);
	if (err)
		return err;

	sb->s_maxbytes		= MAX_LFS_FILESIZE;
	sb->s_blocksize		= PAGE_SIZE;
	sb->s_blocksize_bits	= PAGE_SHIFT;
	sb->s_magic		= BOPFS_MAGIC;
	sb->s_op		= &bopfs_ops;
	sb->s_time_gran		= 1;

	err = kern_path(fsi->mount_opts.backend_path, LOOKUP_FOLLOW | LOOKUP_DIRECTORY, &path);
	if(err)
		return err;
	
	if(!S_ISDIR(path.dentry->d_inode->i_mode)) {
		printk(KERN_ERR "bopfs: backend path: %s must be a directory\n", fsi->mount_opts.backend_path);
		path_put(&path);
		return -EINVAL;
	}

	fsi->b_mount = path.mnt;

	inode = bopfs_iget(sb, NULL, NULL, S_IFDIR | fsi->mount_opts.mode);
	sb->s_root = d_make_root(inode);
	if (!sb->s_root) {
		path_put(&path);
		return -ENOMEM;
	}

	pi = get_bopfs_inode(inode);
	pi->p_dentry = sb->s_root;
	
	path_put(&path);

	return 0;
}

static struct dentry *bopfs_mount(struct file_system_type *fs_type,
		       int flags, const char *dev_name,
		       void *raw_data)
{
	PRINTFN;
	return mount_nodev(fs_type, flags, raw_data, bopfs_fill_super);
}

static void bopfs_kill_sb(struct super_block *sb)
{
	PRINTFN;
	kfree(((struct bopfs_fs_info *)sb->s_fs_info)->mount_opts.backend_path);
	kfree(((struct bopfs_fs_info *)sb->s_fs_info)->mount_opts.user_library_path);
	kfree(sb->s_fs_info);
	kill_anon_super(sb);
}

static struct file_system_type bopfs_fs_type = {
	.owner		= THIS_MODULE,
	.name		= "bopfs",
	.fs_flags	= FS_USERNS_MOUNT,
	.mount		= bopfs_mount,
	.kill_sb	= bopfs_kill_sb,
};
MODULE_ALIAS_FS("bopfs");

static void bopfs_inode_init_once(void *foo)
{
	struct inode *inode = foo;

	inode_init_once(inode);
}

static int __init bopfs_init(void)
{
	static unsigned long once;

	if (test_and_set_bit(0, &once))
		return 0;
	
	printk(KERN_INFO "bopfs init\n");

	bopfs_inode_cachep = kmem_cache_create("bopfs_inode",
		  					  sizeof(struct bopfs_inode),
					    	  0, SLAB_HWCACHE_ALIGN,
					    	  bopfs_inode_init_once);

	return register_filesystem(&bopfs_fs_type);
	return 0;
}

static void __exit bopfs_exit(void)
{
	printk(KERN_DEBUG "bopfs exit\n");
	unregister_filesystem(&bopfs_fs_type);

	/*
	 * Make sure all delayed rcu free inodes are flushed before we
	 * destroy cache.
	 */
	rcu_barrier();
	kmem_cache_destroy(bopfs_inode_cachep);
}

module_init(bopfs_init);
module_exit(bopfs_exit);

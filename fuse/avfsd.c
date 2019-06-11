/*
    FUSE: Filesystem in Userspace
    Copyright (C) 2001  Miklos Szeredi <miklos@szeredi.hu>
    Copyright (C) 2009-2010  Ralf Hoffmann (ralf@boomerangsworld.de)

    This program can be distributed under the terms of the GNU GPL.
    See the file COPYING.
*/

#define FUSE_USE_VERSION 26

#include <fuse.h>
#include <virtual.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <pthread.h>

struct fuse *fuse;

static pthread_mutex_t avfsd_mutexlock = PTHREAD_MUTEX_INITIALIZER;

static int avfsd_getattr(const char *path, struct stat *stbuf)
{
    int res;

    res = virt_lstat(path, stbuf);
    if (res == -1)
        return -errno;

    return 0;
}

static int avfsd_readlink(const char *path, char *buf, size_t size)
{
    int res;

    res = virt_readlink(path, buf, size - 1);
    if (res == -1)
        return -errno;

    buf[res] = '\0';
    return 0;
}


static int avfsd_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
                         off_t offset, struct fuse_file_info *fi)
{
    DIR *dp;
    struct dirent *de;

    (void) offset;
    (void) fi;
    dp = virt_opendir(path);
    if (dp == NULL)
        return -errno;

    while((de = virt_readdir(dp)) != NULL) {
        struct stat st;
        memset(&st, 0, sizeof(st));
        st.st_ino = de->d_ino;
        st.st_mode = de->d_type << 12;
        if (filler(buf, de->d_name, &st, 0))
            break;
    }

    virt_closedir(dp);
    return 0;
}

static int avfsd_mknod(const char *path, mode_t mode, dev_t rdev)
{
    int res;

    res = virt_mknod(path, mode, rdev);
    if (res == -1)
        return -errno;

    return 0;
}

static int avfsd_mkdir(const char *path, mode_t mode)
{
    int res;

    res = virt_mkdir(path, mode);
    if (res == -1)
        return -errno;

    return 0;
}

static int avfsd_unlink(const char *path)
{
    int res;

    res = virt_unlink(path);
    if (res == -1)
        return -errno;

    return 0;
}

static int avfsd_rmdir(const char *path)
{
    int res;

    res = virt_rmdir(path);
    if (res == -1)
        return -errno;

    return 0;
}

static int avfsd_symlink(const char *from, const char *to)
{
    int res;

    res = virt_symlink(from, to);
    if (res == -1)
        return -errno;

    return 0;
}

static int avfsd_rename(const char *from, const char *to)
{
    int res;

    res = virt_rename(from, to);
    if (res == -1)
        return -errno;

    return 0;
}

static int avfsd_link(const char *from, const char *to)
{
    int res;

    res = virt_link(from, to);
    if (res == -1)
        return -errno;

    return 0;
}

static int avfsd_chmod(const char *path, mode_t mode)
{
    int res;

    res = virt_chmod(path, mode);
    if (res == -1)
        return -errno;

    return 0;
}

static int avfsd_chown(const char *path, uid_t uid, gid_t gid)
{
    int res;

    res = virt_lchown(path, uid, gid);
    if (res == -1)
        return -errno;

    return 0;
}

static int avfsd_truncate(const char *path, off_t size)
{
    int res;

    res = virt_truncate(path, size);
    if (res == -1)
        return -errno;

    return 0;
}

static int avfsd_utime(const char *path, struct utimbuf *buf)
{
    int res;

    res = virt_utime(path, buf);
    if (res == -1)
        return -errno;

    return 0;
}


static int avfsd_open(const char *path, struct fuse_file_info *fi)
{
    int res;

    res = virt_open(path, fi->flags, 0);
    if (res == -1)
        return -errno;

    fi->fh = res;
    return 0;
}

static int avfsd_read(const char *path, char *buf, size_t size, off_t offset,
                      struct fuse_file_info *fi)
{
    int res;

    (void) path;

    pthread_mutex_lock( &avfsd_mutexlock );
    if (virt_lseek(fi->fh, offset, SEEK_SET) == -1) {
        pthread_mutex_unlock( &avfsd_mutexlock );
        return -errno;
    }

    res = virt_read(fi->fh, buf, size);
    pthread_mutex_unlock( &avfsd_mutexlock );

    if (res == -1)
        return -errno;

    return res;
}

static int avfsd_write(const char *path, const char *buf, size_t size,
                       off_t offset, struct fuse_file_info *fi)
{
    int res;

    (void) path;

    pthread_mutex_lock( &avfsd_mutexlock );
    if (virt_lseek(fi->fh, offset, SEEK_SET) == -1) {
        pthread_mutex_unlock( &avfsd_mutexlock );
        return -errno;
    }

    res = virt_write(fi->fh, buf, size);
    pthread_mutex_unlock( &avfsd_mutexlock );

    if (res == -1)
        return -errno;

    return res;
}

static int avfsd_release(const char *path, struct fuse_file_info *fi)
{
    (void) path;

    pthread_mutex_lock( &avfsd_mutexlock );
    virt_close(fi->fh);
    pthread_mutex_unlock( &avfsd_mutexlock );

    return 0;
}

static int avfsd_access(const char *path, int mask)
{
    int res;

    res = virt_access(path, mask);
    if (res == -1)
        return -errno;

    return 0;
}

static int avfsd_create(const char *path, mode_t mode, struct fuse_file_info *fi)
{
    int res;

    /* open will handle the O_CREAT flag */
    res = virt_open(path, fi->flags | O_CREAT | O_TRUNC, mode);
    if (res == -1)
        return -errno;

    fi->fh = res;
    return 0;
}

static struct fuse_operations avfsd_oper = {
    getattr:	avfsd_getattr,
    readlink:	avfsd_readlink,
    readdir:	avfsd_readdir,
    mknod:	avfsd_mknod,
    mkdir:	avfsd_mkdir,
    symlink:	avfsd_symlink,
    unlink:	avfsd_unlink,
    rmdir:	avfsd_rmdir,
    rename:	avfsd_rename,
    link:	avfsd_link,
    chmod:	avfsd_chmod,
    chown:	avfsd_chown,
    truncate:	avfsd_truncate,
    utime:	avfsd_utime,
    open:	avfsd_open,
    read:	avfsd_read,
    write:	avfsd_write,
    release:	avfsd_release,
    access:	avfsd_access,
    create:	avfsd_create,
};

int main(int argc, char *argv[])
{
    fuse_main(argc, argv, &avfsd_oper, NULL);
    return 0;
}

# DEFUSE

Source code for Delayed Exploration File Systems in User Space (DEFUSE).  A fully user level interface for user space file systems.  Using LD_PRELOAD, a small kernel driver, and existing kernel interfaces provides high speed access to file systems implemented in user space without context switching to the kernel as is done with FUSE.

## Repository Organization

bopfs_module - kernel module implementing bypassed open.  A feature used by DEFUSE to skip all access and lookup calls done by the kernel during the processing of an 'open' syscall.  A file descriptor opened from bopfs cannot be used for any I/O operations.  Meta operations (lseek, fcntl, etc.) are supported.


#ifndef MEM_LFS_MEM_THREAD_ADAPTER_H
#define MEM_LFS_MEM_THREAD_ADAPTER_H

#include "lfs.h"

// Read a region in a block. Negative error codes are propogated
// to the user.
int mem_thread_read(const struct lfs_config *c, lfs_block_t block,
                    lfs_off_t off, void *buffer, lfs_size_t size);

// Program a region in a block. The block must have previously
// been erased. Negative error codes are propogated to the user.
// May return LFS_ERR_CORRUPT if the block should be considered bad.
int mem_thread_prog(const struct lfs_config *c, lfs_block_t block,
                    lfs_off_t off, const void *buffer, lfs_size_t size);

// Erase a block. A block must be erased before being programmed.
// The state of an erased block is undefined. Negative error codes
// are propogated to the user.
// May return LFS_ERR_CORRUPT if the block should be considered bad.
int mem_thread_erase(const struct lfs_config *c, lfs_block_t block);

// Sync the state of the underlying block device. Negative error codes
// are propogated to the user.
int mem_thread_sync(const struct lfs_config *c);

//int mem_thread_open(lfs_t *fs, )
int mem_thread_open(lfs_t *lfs, lfs_file_t *file,
                  const char *path, int flags);

int mem_thread_close(lfs_t *lfs, lfs_file_t *file);

lfs_ssize_t mem_thread_read_f(lfs_t *lfs, lfs_file_t *file,
                          void *buffer, lfs_size_t size);

lfs_ssize_t mem_thread_write_f(lfs_t *lfs, lfs_file_t *file,
                           const void *buffer, lfs_size_t size);

#endif // MEM_LFS_MEM_THREAD_ADAPTER_H

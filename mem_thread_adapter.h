#ifndef MEM_LFS_MEM_THREAD_ADAPTER_H
#define MEM_LFS_MEM_THREAD_ADAPTER_H

#include "lfs.h"

int mem_thread_open(unsigned fs_id, lfs_t *lfs, lfs_file_t *file,
                    const char *path, int flags);

int mem_thread_close(unsigned fs_id, lfs_t *lfs, lfs_file_t *file);

lfs_ssize_t mem_thread_read_f(unsigned fs_id, lfs_t *lfs, lfs_file_t *file,
                              void *buffer, lfs_size_t size);

lfs_ssize_t mem_thread_write_f(unsigned fs_id, lfs_t *lfs, lfs_file_t *file,
                               const void *buffer, lfs_size_t size);

#endif // MEM_LFS_MEM_THREAD_ADAPTER_H

#ifndef MEM_LFS_MEM_THREAD_ADAPTER_H
#define MEM_LFS_MEM_THREAD_ADAPTER_H

#include "lfs.h"

int mem_thread_mount(unsigned fs_id, lfs_t *lfs,
                     const struct lfs_config *config);

int mem_thread_unmount(unsigned fs_id, lfs_t *lfs);

int mem_thread_format(unsigned fs_id, lfs_t *lfs,
                      const struct lfs_config *config);

int mem_thread_remove(unsigned fs_id, lfs_t *lfs, const char *path);

int mem_thread_rename(unsigned fs_id, lfs_t *lfs, const char *oldpath,
                      const char *newpath);

int mem_thread_open(unsigned fs_id, lfs_t *lfs, lfs_file_t *file,
                    const char *path, int flags);

int mem_thread_close(unsigned fs_id, lfs_t *lfs, lfs_file_t *file);

lfs_ssize_t mem_thread_read_f(unsigned fs_id, lfs_t *lfs, lfs_file_t *file,
                              void *buffer, lfs_size_t size);

lfs_ssize_t mem_thread_write_f(unsigned fs_id, lfs_t *lfs, lfs_file_t *file,
                               const void *buffer, lfs_size_t size);

lfs_ssize_t mem_thread_getattr(unsigned fs_id, lfs_t *lfs, const char *path,
                               uint8_t type, void *buffer, lfs_size_t size);

int mem_thread_setattr(unsigned fs_id, lfs_t *lfs, const char *path,
                       uint8_t type, const void *buffer, lfs_size_t size);

int mem_thread_removeattr(unsigned fs_id, lfs_t *lfs, const char *path,
                          uint8_t type);

#endif // MEM_LFS_MEM_THREAD_ADAPTER_H

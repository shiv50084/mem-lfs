#include "mem_thread_adapter.h"
#include "mem.h"

#include <pthread.h>

static pthread_mutex_t global_lock = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t global_lock2 = PTHREAD_MUTEX_INITIALIZER;

#define LOCKED_OP(plock, fun, ...)                                             \
    ({                                                                         \
        pthread_mutex_lock(plock);                                             \
        int __retval = fun(__VA_ARGS__);                                       \
        pthread_mutex_unlock(plock);                                           \
        __retval;                                                              \
    })

static pthread_mutex_t *get_mutex_by_id(unsigned id)
{
    return id == MEM1 ? &global_lock : &global_lock2;
}

int mem_thread_mount(unsigned fs_id, lfs_t *lfs,
                     const struct lfs_config *config)
{
    return LOCKED_OP(get_mutex_by_id(fs_id), lfs_mount, lfs, config);
}

int mem_thread_unmount(unsigned fs_id, lfs_t *lfs)
{
    return LOCKED_OP(get_mutex_by_id(fs_id), lfs_unmount, lfs);
}

int mem_thread_format(unsigned fs_id, lfs_t *lfs,
                      const struct lfs_config *config)
{
    return LOCKED_OP(get_mutex_by_id(fs_id), lfs_format, lfs, config);
}

int mem_thread_remove(unsigned fs_id, lfs_t *lfs, const char *path)
{
    return LOCKED_OP(get_mutex_by_id(fs_id), lfs_remove, lfs, path);
}

int mem_thread_rename(unsigned fs_id, lfs_t *lfs, const char *oldpath,
                      const char *newpath)
{
    return LOCKED_OP(get_mutex_by_id(fs_id), lfs_rename, lfs, oldpath, newpath);
}

int mem_thread_open(unsigned fs_id, lfs_t *lfs, lfs_file_t *file,
                    const char *path, int flags)
{
    return LOCKED_OP(get_mutex_by_id(fs_id), lfs_file_open, lfs, file, path,
                     flags);
}

int mem_thread_close(unsigned fs_id, lfs_t *lfs, lfs_file_t *file)
{
    return LOCKED_OP(get_mutex_by_id(fs_id), lfs_file_close, lfs, file);
}

lfs_ssize_t mem_thread_read_f(unsigned fs_id, lfs_t *lfs, lfs_file_t *file,
                              void *buffer, lfs_size_t size)
{
    return LOCKED_OP(get_mutex_by_id(fs_id), lfs_file_read, lfs, file, buffer,
                     size);
}

lfs_ssize_t mem_thread_write_f(unsigned fs_id, lfs_t *lfs, lfs_file_t *file,
                               const void *buffer, lfs_size_t size)
{
    return LOCKED_OP(get_mutex_by_id(fs_id), lfs_file_write, lfs, file, buffer,
                     size);
}

lfs_ssize_t mem_thread_getattr(unsigned fs_id, lfs_t *lfs, const char *path,
                               uint8_t type, void *buffer, lfs_size_t size)
{
    return LOCKED_OP(get_mutex_by_id(fs_id), lfs_getattr, lfs, path, type,
                     buffer, size);
}

int mem_thread_setattr(unsigned fs_id, lfs_t *lfs, const char *path,
                       uint8_t type, const void *buffer, lfs_size_t size)
{
    return LOCKED_OP(get_mutex_by_id(fs_id), lfs_setattr, lfs, path, type,
                     buffer, size);
}

int mem_thread_removeattr(unsigned fs_id, lfs_t *lfs, const char *path,
                          uint8_t type)
{
    return LOCKED_OP(get_mutex_by_id(fs_id), lfs_removeattr, lfs, path, type);
}

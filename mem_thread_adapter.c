#include "mem_thread_adapter.h"
#include "mem.h"

#include <pthread.h>

static pthread_mutex_t mem_lock = PTHREAD_MUTEX_INITIALIZER;

#define LOCKED_OP(lock, fun, ...)                                              \
    ({                                                                         \
        pthread_mutex_lock(&lock);                                             \
        int __retval = fun(__VA_ARGS__);                                       \
        pthread_mutex_unlock(&lock);                                           \
        __retval;                                                              \
    })

// Read a region in a block. Negative error codes are propogated
// to the user.
int mem_thread_read(const struct lfs_config *c, lfs_block_t block,
                    lfs_off_t off, void *buffer, lfs_size_t size)
{
    return LOCKED_OP(mem_lock, memory_read, c, block, off, buffer, size);
}

// Program a region in a block. The block must have previously
// been erased. Negative error codes are propogated to the user.
// May return LFS_ERR_CORRUPT if the block should be considered bad.
int mem_thread_prog(const struct lfs_config *c, lfs_block_t block,
                    lfs_off_t off, const void *buffer, lfs_size_t size)
{
    return LOCKED_OP(mem_lock, memory_prog, c, block, off, buffer, size);
}

// Erase a block. A block must be erased before being programmed.
// The state of an erased block is undefined. Negative error codes
// are propogated to the user.
// May return LFS_ERR_CORRUPT if the block should be considered bad.
int mem_thread_erase(const struct lfs_config *c, lfs_block_t block)
{
    return LOCKED_OP(mem_lock, memory_erase, c, block);
}

// Sync the state of the underlying block device. Negative error codes
// are propogated to the user.
int mem_thread_sync(const struct lfs_config *c)
{
    return LOCKED_OP(mem_lock, memory_sync, c);
}

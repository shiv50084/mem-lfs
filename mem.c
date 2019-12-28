#include "mem.h"

#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

static unsigned char memory_storage[MEMORY_STORAGE_SIZE];
static unsigned char memory_storage2[MEMORY_STORAGE_SIZE];

static inline size_t get_address(size_t block_size, lfs_block_t block)
{
    size_t base_offset = block_size * block;
    assert(base_offset < MEMORY_STORAGE_SIZE);
    return base_offset;
}

// Read a region in a block. Negative error codes are propogated
// to the user.
int memory_read(const struct lfs_config *c, lfs_block_t block, lfs_off_t off,
                void *buffer, lfs_size_t size)
{
    lfs_off_t base_offset = get_address(c->block_size, block) + off;
    // printf("%s: base offset: %u\n", __func__, base_offset);
    unsigned char *_memory = c->context;
    memcpy(buffer, _memory + base_offset, size);

    return LFS_ERR_OK;
}

// Program a region in a block. The block must have previously
// been erased. Negative error codes are propogated to the user.
// May return LFS_ERR_CORRUPT if the block should be considered bad.
int memory_prog(const struct lfs_config *c, lfs_block_t block, lfs_off_t off,
                const void *buffer, lfs_size_t size)
{
    lfs_off_t base_offset = get_address(c->block_size, block) + off;
    // printf("%s: base offset: %u\n", __func__, base_offset);
    unsigned char *_memory = c->context;
    memcpy(_memory + base_offset, buffer, size);

    return LFS_ERR_OK;
}

// Erase a block. A block must be erased before being programmed.
// The state of an erased block is undefined. Negative error codes
// are propogated to the user.
// May return LFS_ERR_CORRUPT if the block should be considered bad.
int memory_erase(const struct lfs_config *c, lfs_block_t block)
{
    lfs_off_t base_offset = get_address(c->block_size, block);
    // printf("%s: base offset: %u\n", __func__, base_offset);
    // memset(memory_storage + base_offset, 0xff, c->block_size);

    return LFS_ERR_OK;
}

// Sync the state of the underlying block device. Negative error codes
// are propogated to the user.
int memory_sync(const struct lfs_config *c)
{
    return LFS_ERR_OK;
}

unsigned char *memory_storage_get(void)
{
    return memory_storage;
}

unsigned char *memory_storage2_get(void)
{
    return memory_storage2;
}

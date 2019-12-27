#include "lfs.h"
#include "memory_utils.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

#define MEMORY_STORAGE_SIZE (32U * 1024)
#define MEMORY_PAGE_SIZE (256UL)
#define MEMORY_BLOCK_SIZE (2048UL)
#define MEMORY_BLOCK_COUNT (MEMORY_STORAGE_SIZE / MEMORY_BLOCK_SIZE)

static unsigned char memory_storage[MEMORY_STORAGE_SIZE];

static unsigned char lfs_prog_buffer[MEMORY_PAGE_SIZE];
static unsigned char lfs_read_buffer[MEMORY_PAGE_SIZE];
static unsigned char lfs_lookahead_buffer[MEMORY_PAGE_SIZE];

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
    printf("%s: base offset: %u\n", __func__, base_offset);
    memcpy(buffer, memory_storage + base_offset, size);

    return LFS_ERR_OK;
}

// Program a region in a block. The block must have previously
// been erased. Negative error codes are propogated to the user.
// May return LFS_ERR_CORRUPT if the block should be considered bad.
int memory_prog(const struct lfs_config *c, lfs_block_t block, lfs_off_t off,
                const void *buffer, lfs_size_t size)
{
    lfs_off_t base_offset = get_address(c->block_size, block) + off;
    printf("%s: base offset: %u\n", __func__, base_offset);
    memcpy(memory_storage + base_offset, buffer, size);

    return LFS_ERR_OK;
}

// Erase a block. A block must be erased before being programmed.
// The state of an erased block is undefined. Negative error codes
// are propogated to the user.
// May return LFS_ERR_CORRUPT if the block should be considered bad.
int memory_erase(const struct lfs_config *c, lfs_block_t block)
{
    lfs_off_t base_offset = get_address(c->block_size, block);
    printf("%s: base offset: %u\n", __func__, base_offset);
    //memset(memory_storage + base_offset, 0xff, c->block_size);

    return LFS_ERR_OK;
}

// Sync the state of the underlying block device. Negative error codes
// are propogated to the user.
int memory_sync(const struct lfs_config *c)
{
    return LFS_ERR_OK;
}

static struct lfs_config cfg = {
    // block device operations
    .read = memory_read,
    .prog = memory_prog,
    .erase = memory_erase,
    .sync = memory_sync,

    // block device configuration
    .read_size = MEMORY_PAGE_SIZE,
    .prog_size = MEMORY_PAGE_SIZE,
    .block_size = MEMORY_BLOCK_SIZE,
    .block_count = MEMORY_BLOCK_COUNT,
    .cache_size = MEMORY_PAGE_SIZE,
    .lookahead_size = MEMORY_PAGE_SIZE,
    .block_cycles = 100,
    .read_buffer = lfs_read_buffer,
    .prog_buffer = lfs_prog_buffer,
    .lookahead_buffer = lfs_lookahead_buffer};

int main()
{
    lfs_t fs;
    lfs_file_t file = {0};

    if (memory_utils_restore_dump("memdump", memory_storage, sizeof memory_storage) != MEM_OK)
    {
        printf("%s: Unable to restore memory dump\n", __func__);
    }

    if (lfs_mount(&fs, &cfg) != LFS_ERR_OK)
    {
        lfs_format(&fs, &cfg);

        if (lfs_mount(&fs, &cfg) != LFS_ERR_OK)
        {
            return 1;
        }
    }

    if (memory_utils_save_dump("memdump", memory_storage, sizeof memory_storage) != MEM_OK)
    {
        printf("%s: Unable to save memory dump\n", __func__);
    }

    return 0;
}

#ifndef MEM_LFS_MEM_H
#define MEM_LFS_MEM_H

#include "lfs.h"

#define MEMORY_STORAGE_SIZE (32U * 1024)
#define MEMORY_PAGE_SIZE (256UL)
#define MEMORY_BLOCK_SIZE (2048UL)
#define MEMORY_BLOCK_COUNT (MEMORY_STORAGE_SIZE / MEMORY_BLOCK_SIZE)

#define MEM1 1
#define MEM2 2

int memory_read(const struct lfs_config *c, lfs_block_t block, lfs_off_t off,
                void *buffer, lfs_size_t size);

// Program a region in a block. The block must have previously
// been erased. Negative error codes are propogated to the user.
// May return LFS_ERR_CORRUPT if the block should be considered bad.
int memory_prog(const struct lfs_config *c, lfs_block_t block, lfs_off_t off,
                const void *buffer, lfs_size_t size);

// Erase a block. A block must be erased before being programmed.
// The state of an erased block is undefined. Negative error codes
// are propogated to the user.
// May return LFS_ERR_CORRUPT if the block should be considered bad.
int memory_erase(const struct lfs_config *c, lfs_block_t block);

// Sync the state of the underlying block device. Negative error codes
// are propogated to the user.
int memory_sync(const struct lfs_config *c);

unsigned char *memory_storage_get(void);
unsigned char *memory_storage2_get(void);

struct lfs_config_init
{
    int block_cycles;
    unsigned block_size;
    unsigned block_count;
    unsigned read_size;
    unsigned prog_size;
    unsigned cache_size;
    unsigned lookahead_size;
    unsigned char *prog_buffer;
    unsigned char *lookahead_buffer;
    unsigned char *read_buffer;
};

int memory_storage_config_init(struct lfs_config *c,
                               const struct lfs_config_init *conf_init);

#endif // MEM_LFS_MEM_H

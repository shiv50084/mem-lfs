#include "lfs.h"
#include "mem.h"
#include "mem_thread_adapter.h"
#include "memory_utils.h"

#include <stdio.h>

#define MEMORY_STORAGE_SIZE (32U * 1024)
#define MEMORY_PAGE_SIZE (256UL)
#define MEMORY_BLOCK_SIZE (2048UL)
#define MEMORY_BLOCK_COUNT (MEMORY_STORAGE_SIZE / MEMORY_BLOCK_SIZE)

static unsigned char lfs_prog_buffer[MEMORY_PAGE_SIZE];
static unsigned char lfs_read_buffer[MEMORY_PAGE_SIZE];
static unsigned char lfs_lookahead_buffer[MEMORY_PAGE_SIZE];

static struct lfs_config cfg = {
    // block device operations
    .read = mem_thread_read,
    .prog = mem_thread_prog,
    .erase = mem_thread_erase,
    .sync = mem_thread_sync,

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

    if (memory_utils_restore_dump("memdump", memory_storage_get(),
                                  MEMORY_STORAGE_SIZE) != MEM_OK)
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

    if (memory_utils_save_dump("memdump", memory_storage_get(),
                               MEMORY_STORAGE_SIZE) != MEM_OK)
    {
        printf("%s: Unable to save memory dump\n", __func__);
    }

    return 0;
}

#include "lfs.h"
#include "mem.h"
#include "mem_thread_adapter.h"
#include "memory_utils.h"

#include <pthread.h>

#include <stdio.h>
#include <string.h>

static unsigned char lfs_prog_buffer[MEMORY_PAGE_SIZE];
static unsigned char lfs_read_buffer[MEMORY_PAGE_SIZE];
static __attribute__((aligned(32))) unsigned char lfs_lookahead_buffer[MEMORY_PAGE_SIZE];

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

static void *task_1_handler(void *arg);
static void *task_2_handler(void *arg);

lfs_t fs;

int main()
{
    pthread_t task_1;
    pthread_t task_2;

    printf("%s: Lookahead buffer addr: %p\n", __func__, lfs_lookahead_buffer);

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

    pthread_create(&task_1, NULL, task_1_handler, NULL);
    pthread_create(&task_2, NULL, task_2_handler, NULL);
    pthread_join(task_1, NULL);
    pthread_join(task_2, NULL);

    if (memory_utils_save_dump("memdump", memory_storage_get(),
                               MEMORY_STORAGE_SIZE) != MEM_OK)
    {
        printf("%s: Unable to save memory dump\n", __func__);
    }

    return 0;
}

static int file_write(const char *filename, const unsigned char *buf,
                      size_t buf_size, lfs_file_t *file)
{
    int retval = 0;
    if ((retval = mem_thread_open(&fs, file, filename,
                                LFS_O_CREAT | LFS_O_WRONLY)) != LFS_ERR_OK)
    {
        printf("%s: File open err [%d]\n", __func__, retval);
        goto exit;
    }

    retval = mem_thread_write_f(&fs, file, buf, buf_size);

    if (retval < 0)
    {
        printf("%s: File write error [%d]\n", __func__, retval);
    }

    retval = mem_thread_close(&fs, file);
    printf("%s: File close error [%d]\n", __func__, retval);
exit:
    return retval;
}

static int file_read(const char *filename, unsigned char *buf, size_t buf_size,
                     lfs_file_t *file)
{
    int retval = 0;
    if ((retval = mem_thread_open(&fs, file, filename, LFS_O_RDONLY)) !=
        LFS_ERR_OK)
    {
        printf("%s: File open err [%d]\n", __func__, retval);
        goto exit;
    }

    retval = mem_thread_read_f(&fs, file, buf, buf_size);

    if (retval < 0)
    {
        printf("%s: File write error [%d]\n", __func__, retval);
    }

    retval = mem_thread_close(&fs, file);
    printf("%s: File close error [%d]\n", __func__, retval);
exit:
    return retval;
}

static void *task_1_handler(void *arg)
{
    lfs_file_t file = {0};
    const char *str = "Hello";
    char buf[128];

    for (size_t i = 0; i < 128 * 64; ++i)
    {
        int retval = file_write("file1", str, strlen(str), &file);

        if (retval != LFS_ERR_OK)
        {
            printf("%s: Write error", __func__);
            goto exit;
        }

        retval = file_read("file1", buf, strlen(str), &file);

        if (retval != LFS_ERR_OK)
        {
            printf("%s: Read error", __func__);
            goto exit;
        }

        if (strncmp(buf, str, strlen(str)) != 0)
        {
            printf("%s: Content mismatch\n", __func__);
        }
        else
        {
            printf("%s: Content equal\n", __func__);
        }
    }
exit:
    pthread_exit(NULL);
}

static void *task_2_handler(void *arg)
{
    lfs_file_t file = {0};
    const char *str = "World";
    char buf[128];

    for (size_t i = 0; i < 128 * 64; ++i)
    {
        int retval = file_write("file2", str, strlen(str), &file);

        if (retval != LFS_ERR_OK)
        {
            printf("%s: Write error", __func__);
            goto exit;
        }

        retval = file_read("file2", buf, strlen(str), &file);

        if (retval != LFS_ERR_OK)
        {
            printf("%s: Read error", __func__);
            goto exit;
        }

        if (strncmp(buf, str, strlen(str)) != 0)
        {
            printf("%s: Content mismatch\n", __func__);
        }
        else
        {
            printf("%s: Content equal\n", __func__);
        }
    }
exit:
    pthread_exit(NULL);
}
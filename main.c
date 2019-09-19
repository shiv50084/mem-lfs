#include "lfs.h"
#include "mem.h"
#include "mem_thread_adapter.h"
#include "memory_utils.h"

#include <pthread.h>

#include <stdio.h>
#include <string.h>

static unsigned char lfs_prog_buffer[MEMORY_PAGE_SIZE];
static unsigned char lfs_read_buffer[MEMORY_PAGE_SIZE];
static unsigned char lfs_lookahead_buffer[MEMORY_PAGE_SIZE];

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

static unsigned char lfs_prog_buffer2[MEMORY_PAGE_SIZE];
static unsigned char lfs_read_buffer2[MEMORY_PAGE_SIZE];
static unsigned char lfs_lookahead_buffer2[MEMORY_PAGE_SIZE];

static struct lfs_config cfg2 = {
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
    .read_buffer = lfs_read_buffer2,
    .prog_buffer = lfs_prog_buffer2,
    .lookahead_buffer = lfs_lookahead_buffer2};

static void *task_1_handler(void *arg);
static void *task_2_handler(void *arg);
static void *task_3_handler(void *arg);
static void *task_4_handler(void *arg);

lfs_t fs;
lfs_t fs2;

static int init_fs(void)
{
    cfg.context = memory_storage_get();
    if (mem_thread_mount(MEM1, &fs, &cfg) != LFS_ERR_OK)
    {
        mem_thread_format(MEM1, &fs, &cfg);

        if (mem_thread_mount(MEM1, &fs, &cfg) != LFS_ERR_OK)
        {
            return 1;
        }
    }

    cfg2.context = memory_storage2_get();
    if (mem_thread_mount(MEM2, &fs2, &cfg2) != LFS_ERR_OK)
    {
        mem_thread_format(MEM2, &fs2, &cfg2);

        if (mem_thread_mount(MEM2, &fs2, &cfg2) != LFS_ERR_OK)
        {
            return 2;
        }
    }

    return 0;
}

int main()
{
    pthread_t task_1;
    pthread_t task_2;
    pthread_t task_3;
    pthread_t task_4;

    printf("%s: Lookahead buffer addr: %p\n", __func__, lfs_lookahead_buffer);

    if (memory_utils_restore_dump("memdump", memory_storage_get(),
                                  MEMORY_STORAGE_SIZE) != MEM_OK)
    {
        printf("%s: Unable to restore memory dump\n", __func__);
    }

    if (memory_utils_restore_dump("memdump2", memory_storage2_get(),
                                  MEMORY_STORAGE_SIZE) != MEM_OK)
    {
        printf("%s: Unable to restore memory dump\n", __func__);
    }

    if (init_fs() != 0)
    {
        return 2;
    }

    pthread_create(&task_1, NULL, task_1_handler, NULL);
    pthread_create(&task_2, NULL, task_2_handler, NULL);
    pthread_create(&task_3, NULL, task_3_handler, NULL);
    pthread_create(&task_4, NULL, task_4_handler, NULL);
    pthread_join(task_1, NULL);
    pthread_join(task_2, NULL);
    pthread_join(task_3, NULL);
    pthread_join(task_4, NULL);

    int ret = lfs_unmount(&fs);

    if (ret != LFS_ERR_OK)
    {
        printf("%s: Unmounting error [%d]", __func__, ret);
    }

    ret = lfs_unmount(&fs2);

    if (ret != LFS_ERR_OK)
    {
        printf("%s: Unmounting error [%d]", __func__, ret);
    }

    if (memory_utils_save_dump("memdump", memory_storage_get(),
                               MEMORY_STORAGE_SIZE) != MEM_OK)
    {
        printf("%s: Unable to save memory dump\n", __func__);
    }

    if (memory_utils_save_dump("memdump2", memory_storage2_get(),
                               MEMORY_STORAGE_SIZE) != MEM_OK)
    {
        printf("%s: Unable to save memory dump\n", __func__);
    }

    return 0;
}

static int file_write(unsigned fs_id, const char *filename,
                      const unsigned char *buf, size_t buf_size,
                      lfs_file_t *file)
{
    lfs_t *pfs = fs_id == MEM1 ? &fs : &fs2;
    int retval = 0;
    if ((retval = mem_thread_open(fs_id, pfs, file, filename,
                                  LFS_O_CREAT | LFS_O_WRONLY)) != LFS_ERR_OK)
    {
        if (retval != LFS_ERR_OK)
        {
            printf("%s: File open err [%d]\n", __func__, retval);
        }
        goto exit;
    }

    retval = mem_thread_write_f(fs_id, pfs, file, buf, buf_size);

    if (retval < 0)
    {
        printf("%s: File write error [%d]\n", __func__, retval);
    }

    retval = mem_thread_close(fs_id, pfs, file);

    if (retval != LFS_ERR_OK)
    {
        printf("%s: File close error [%d]\n", __func__, retval);
    }
exit:
    return retval;
}

static int file_read(unsigned fs_id, const char *filename, unsigned char *buf,
                     size_t buf_size, lfs_file_t *file)
{
    lfs_t *pfs = fs_id == MEM1 ? &fs : &fs2;
    int retval = 0;
    if ((retval = mem_thread_open(fs_id, pfs, file, filename, LFS_O_RDONLY)) !=
        LFS_ERR_OK)
    {
        if (retval != LFS_ERR_OK)
        {
            printf("%s: File open err [%d]\n", __func__, retval);
        }
        goto exit;
    }

    retval = mem_thread_read_f(fs_id, pfs, file, buf, buf_size);

    if (retval < 0)
    {
        printf("%s: File write error [%d]\n", __func__, retval);
    }

    retval = mem_thread_close(fs_id, pfs, file);

    if (retval != LFS_ERR_OK)
    {
        printf("%s: File close error [%d]\n", __func__, retval);
    }
exit:
    return retval;
}

static void *task_1_handler(void *arg)
{
    lfs_file_t file = {0};
    const char *str =
        "MlIsJDyGzTpiAmulhjpQloAPtipTtRqEsJQvvByhLQFfBlyXpwsCZsLYEFNyGsGmFwvzOB"
        "JfOioepXqRLEQniPRPmbIuXgnYZuvMWvouWwVVzMUbUuTgLDvgDcdfNtJkofvMCjjvsxUf"
        "CmvFpFMzckQVHYZhToCweYrVokOLveDBFcDEChEJyNqmqQNKywmHIEHuvmSlvyKVAGqGsO"
        "NyMcrRevYKVCkrewXZduKStmpcvgNlweMdNnOabXIkeXgkVhWgrlmJujRqNKQDpTggmTsu"
        "vfuYoqJSaDdfncExHeljHgiohILWUCXLvLYQWkMXiyWLYWGgwdzQNwNyjrxofPaJMawxCP"
        "CGnVDSpntdagMqEthxpGsjLsgmNmoPXOMXuMTAHfOQhXiQCezuKGUDraGoOFwjTPhJhgbk"
        "eejXKiGqZEegaMXqWDUUfqftvArCYURPjxDOHtkhmmjMGzHNwWiSxczpMkbRGtyfStTkhb"
        "yjMhTAjEVdgzCXMqFWtCUd";
    char buf[512];

    for (size_t i = 0; i < 128 * 64; ++i)
    {
        int retval = file_write(MEM1, "file1", str, strlen(str), &file);

        if (retval != LFS_ERR_OK)
        {
            printf("%s: Write error", __func__);
            goto exit;
        }

        retval = file_read(MEM1, "file1", buf, strlen(str), &file);

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
    const char *str =
        "qvAGTsWClryChbcMZxwaYTJrRFNWMxtREnKiCMRKrJbNrIFLmocLgNISBWWinLMTYjscrc"
        "UWmusYlJhmHqGKoeoqFUGGImKimFLVJjTswVczbhYsJKiANvUfdTSFXoelaavJWNjxoxTo"
        "fjYfLhddTlDAsdOXAWtCrxglciVNADFDrZyfqymlziZnOnSjYkWQfFKEiKKcEhVpEoVzPz"
        "YtpyREXlYOlnsTjXNYxIsrvlhJWylSPxAUgslWoQiqFNXKFtYcFMJbazGcPxUDeoZTnvvk"
        "nMJUCLFjZIkKxaixspVwpuKXEfbdMVQuDckOGlWNRJPcntZtiIgGYHBmyhvIAupklUsatG"
        "uFOFoWRWLmjWQONDLSHFiXtWdVawxssuqgzFVwvsuOsYduICFgZwuHSnWBdLYLvffOPhIV"
        "rqHofEqgqEgGsFjvsHpbKWusdAdTPNYaMxlwrQIwnVqtFggGwcRIlFCHGffHQoFxFFfuFj"
        "mVzUIItqKebsjNSzcPjSuK";
    char buf[512];

    for (size_t i = 0; i < 128 * 64; ++i)
    {
        int retval = file_write(MEM1, "file2", str, strlen(str), &file);

        if (retval != LFS_ERR_OK)
        {
            printf("%s: Write error", __func__);
            goto exit;
        }

        retval = file_read(MEM1, "file2", buf, strlen(str), &file);

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

static void *task_3_handler(void *arg)
{
    lfs_file_t file = {0};
    const char *str =
        "CMYEjfETHjrUEFdubLNGBzdXAAiWjKyffIlNarHIZWtYiErelbDyLpvmaZDleMtufYttgS"
        "fSSwPUknAYClqYkeJjzZEuOoZfZEdrEQDMfsEVsxnunnatnWCTHWYKbceDlzocpOBLkXep"
        "JDMfrNHQIPyGRwPDAUPVFfAcyyrhWEzVBZvnOiVYzXzpsqhNPWNBfnAnPhrxwtqPeNumzF"
        "CXTkEgJcbbPguLrmWdnwlhqlyKEJNRIpAoLwqgUmciAXVLyWQmrikYhvMHokKcXlGxIHqL"
        "UrlYaPMVecUKwFQEdxmnYlyhRTnDPaKpHlWQphLnxfRLoIjqtcAootaAFJLUMGPppIXQTt"
        "dWkrqzmsWXxgnENUvkmeWGzoeGRWMaOcoNFXfhsNVwKrngffkAMIzBtpRZRZMlVRsyCwZx"
        "XfJbdgVlehCuHXDYaAlnyilzAJXThNCJcPOugGJlfLsxldarhPYPbJEMJMBJDKIYcTmwEL"
        "SSOvgfDZqjxdTeeCgyCXhC";
    char buf[512];

    for (size_t i = 0; i < 128 * 64; ++i)
    {
        int retval = file_write(MEM2, "file1", str, strlen(str), &file);

        if (retval != LFS_ERR_OK)
        {
            printf("%s: Write error", __func__);
            goto exit;
        }

        retval = file_read(MEM2, "file1", buf, strlen(str), &file);

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

static void *task_4_handler(void *arg)
{
    lfs_file_t file = {0};
    const char *str =
        "NRaECCaLKckAmiVZrfzVwHyLKpwTUGDnczcPgQysEFHlhfNGivZRaoTvpuWerJXTBcxFJA"
        "VqLLFOTQsPdFsKnKFiGbKwITeriyftwIGrCbAtmuIRZSghiDbdsVDhZGjmBSLeIvZaPWUl"
        "uTtFYOiHmfLJTxBWYswDBfQWPrXolbShTtUnHTEFMmJUbctVnMYgZmfTAqWnRZDnuZGsMb"
        "vjzayrIWIPBovUQvmtPrLOSvxuyjuqTChiyxrVHyiUbNrmfQktCzGvKFGYOTCcwWApRwfc"
        "auGcLzyTzRRdVIWEQiwqLposZNRRZwivIeWIKGkowmMwpmPlJeMHcHpgurQssNUHClqoJv"
        "iImBwIykHZmgLVcyuUvRsoTXXnezEgIHQcbyRLiunDclvuuzDejyXyghKCaMbtyYYHMTDn"
        "DmnFfJAOPeCKudySzmTznsBmnbhMsLkfwbxMgrsXBsbVvDliiCPAdGAONPRWXwZkrOZGEe"
        "PkUrmIBHXJjNJxJIndlSfh";
    char buf[512];

    for (size_t i = 0; i < 128 * 64; ++i)
    {
        int retval = file_write(MEM2, "file2", str, strlen(str), &file);

        if (retval != LFS_ERR_OK)
        {
            printf("%s: Write error", __func__);
            goto exit;
        }

        retval = file_read(MEM2, "file2", buf, strlen(str), &file);

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

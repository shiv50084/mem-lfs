#ifndef MEM_LFS_MEMORY_UTILS_H
#define MEM_LFS_MEMORY_UTILS_H

#include <stddef.h>

enum
{
    MEM_OK = 0,
    MEM_INV_ARGS = -1,
    MEM_SAVE_ERR = -2,
    MEM_LOAD_ERR = -3
};

int memory_utils_save_dump(const char *filename, const unsigned char *memory,
                           size_t memory_size);
int memory_utils_restore_dump(const char *filename, unsigned char *memory,
                              size_t memory_size);
void memory_utils_print(const unsigned char *memory, size_t memory_size);

#endif // MEM_LFS_MEMORY_UTILS_H

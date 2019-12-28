#include "memory_utils.h"
#include "print_utils.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define min(a, b)                                                              \
    ({                                                                         \
        __typeof__(a) _a = (a);                                                \
        __typeof__(b) _b = (b);                                                \
        _a < _b ? _a : _b;                                                     \
    })

int memory_utils_save_dump(const char *filename, const unsigned char *memory,
                           size_t memory_size)
{
    if (!filename || !memory || !memory_size)
    {
        return MEM_INV_ARGS;
    }

    FILE *f = fopen(filename, "w");

    if (!f)
    {
        return MEM_SAVE_ERR;
    }

    size_t w_bytes = fwrite(memory, sizeof(unsigned char), memory_size, f);
    assert(w_bytes == memory_size);
    fclose(f);

    return MEM_OK;
}

int memory_utils_restore_dump(const char *filename, unsigned char *memory,
                              size_t memory_size)
{
    if (!filename || !memory || !memory_size)
    {
        return MEM_INV_ARGS;
    }

    FILE *f = fopen(filename, "r");

    if (!f)
    {
        return MEM_LOAD_ERR;
    }

    size_t r_bytes = fread(memory, sizeof(unsigned char), memory_size, f);
    assert(r_bytes == memory_size);
    fclose(f);

    return MEM_OK;
}
void memory_utils_print(const unsigned char *memory, size_t memory_size)
{
    char buf[128];
    size_t offset = 0;

    while (memory_size)
    {
        size_t to_copy = min(memory_size, sizeof buf);
        memcpy(buf, memory + offset, to_copy);
        offset += to_copy;
        memory_size -= to_copy;

        char *repr = array_to_hex((const unsigned char *)buf, sizeof buf);

        if (repr)
        {
            puts(repr);
            free(repr);
        }
    }
}

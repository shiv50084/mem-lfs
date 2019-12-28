#include "print_utils.h"

#include <stdio.h>
#include <stdlib.h>

char *array_to_hex(const unsigned char *arr, size_t arr_size)
{
    if (!arr || !arr_size)
    {
        return NULL;
    }

    size_t repr_length = arr_size * 2 + 1;
    char *repr_arr = malloc(repr_length);

    if (!repr_arr)
    {
        return NULL;
    }

    for (size_t i = 0; i < arr_size; ++i)
    {
        sprintf(repr_arr + 2 * i, "%02hhx", arr[i]);
    }

    repr_arr[repr_length] = '\0';

    return repr_arr;
}

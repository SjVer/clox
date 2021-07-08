#include <stdlib.h>
#include <stdio.h>

#include "memory.h"

// reallocates memory of size newSize for pointer
// if newSize is 0, pointer is freed
void *reallocate(void *pointer, size_t oldSize, size_t newSize)
{
    if (newSize == 0)
    {
        free(pointer);
        return NULL;
    }

    void *result = realloc(pointer, newSize);
    if (result == NULL)
    {
        // exit if we have no more memory available
        printf("REALLOCATION FAILED\n");
        exit(1);
    }
    return result;
}
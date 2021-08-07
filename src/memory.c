#include <stdlib.h>
#include <stdio.h>

#include "memory.h"
#include "object.h"
#include "vm.h"

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

// frees a single object
static void freeObject(Obj *object)
{
    switch (object->type)
    {
    case OBJ_STRING:
    {
        ObjString *string = (ObjString *)object;
        FREE_ARRAY(char, string->chars, string->length + 1);
        FREE(ObjString, object);
        break;
    }
    }
}

// frees the VM's objects from memory
void freeObjects()
{
    Obj* object = vm.objects;
    while (object != NULL)
    {
        Obj* next = object->next;
        freeObject(object);
        object = next;
    }
}
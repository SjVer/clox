// disable "type name is not allowed" error
#ifdef __INTELLISENSE__
#pragma diag_suppress 254
#endif

#include <stdio.h>
#include <string.h>

#include "memory.h"
#include "object.h"
#include "value.h"
#include "table.h"
#include "vm.h"

// allocates an Obj (basically the __init__ for an obj)
#define ALLOCATE_OBJ(type, objectType) \
	(type *)allocateObject(sizeof(type), objectType)

// helper for ALLOCATE_OBJ
static Obj *allocateObject(size_t size, ObjType type)
{
	Obj *object = (Obj *)reallocate(NULL, 0, size);
	object->type = type;

	object->next = vm.objects;
	vm.objects = object;
	return object;
}

// allocates a ObjString
static ObjString *allocateString(char *chars, int length, uint32_t hash)
{
	ObjString *string = ALLOCATE_OBJ(ObjString, OBJ_STRING);
	string->length = length;
	string->chars = chars;
	tableSet(&vm.strings, string, NIL_VAL);
	return string;
}

static uint32_t hashString(const char *key, int length)
{
	uint32_t hash = 2166136261u;
	for (int i = 0; i < length; i++)
	{
		hash ^= (uint8_t)key[i];
		hash *= 16777619;
	}
	return hash;
}

ObjString *takeString(char *chars, int length)
{
	uint32_t hash = hashString(chars, length);

	// check if the string already existed
	ObjString *interned = tableFindString(
		&vm.strings, chars, length, hash);
	if (interned != NULL)
	{
		FREE_ARRAY(char, chars, length + 1);
		return interned;
	}

	return allocateString(chars, length, hash);
}

// copies a c string to a ObjString and returns that
ObjString *copyString(const char *chars, int length)
{
	char *heapChars = ALLOCATE(char, length + 1);
	memcpy(heapChars, chars, length);
	heapChars[length] = '\0';
	uint32_t hash = hashString(chars, length);
	
	// check if the string already existed
	ObjString *interned = tableFindString(
		&vm.strings, chars, length, hash);
	if (interned != NULL) return interned;

	return allocateString(heapChars, length, hash);
}

// prints an Obj
void printObject(Value value)
{
	switch (OBJ_TYPE(value))
	{
	case OBJ_STRING:
		printf("%s", AS_CSTRING(value));
		break;
	}
}
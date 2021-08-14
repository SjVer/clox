#ifndef clox_compiler_h
#define clox_compiler_h

#include "vm.h"

// main compile function
ObjFunction *compile(const char *source);

#endif
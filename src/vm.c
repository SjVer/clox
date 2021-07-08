#include <stdio.h>

#include "compiler.h"
#include "common.h"
#include "debug.h"
#include "vm.h"

VM vm;

// reset the stack
static void resetStack()
{
	vm.stackTop = vm.stack;
}

// initialize the VM
void initVM()
{
	resetStack();
}

// free the VM
void freeVM()
{
}

// run shit
static InterpretResult run()
{
#define READ_BYTE() (*vm.ip++)
#define READ_CONSTANT() (vm.chunk->constants.values[READ_BYTE()])
#define BINARY_OP(op)     \
	do                    \
	{                     \
		double b = pop(); \
		double a = pop(); \
		push(a op b);     \
	} while (false)
	// wrap in block so that the macro expands safely

	for (;;)
	{
// debugging stuff
#ifdef DEBUG_TRACE_EXECUTION
		printf("\n\nSTACK:    ");
		// printf("          ");
		// if (vm.stack[0] == *vm.stackTop)
			// printf("EMPTY");
		for (Value *slot = vm.stack; slot < vm.stackTop; slot++)
		{
			printf("[");
			printValue(*slot);
			printf("]");
		}
		printf("\nINSTRUCT: ");
		disassembleInstruction(vm.chunk, (int)(vm.ip - vm.chunk->code));
		printf(">>> ");
#endif
		// swtich for execution of each intruction
		uint8_t instruction;
		switch (instruction = READ_BYTE())
		{
		case OP_CONSTANT:
		{
			Value constant = READ_CONSTANT();
			push(constant);
			break;
		}
		case OP_ADD:
		{
			BINARY_OP(+);
			break;
		}
		case OP_SUBTRACT:
		{
			BINARY_OP(-);
			break;
		}
		case OP_MULTIPLY:
		{
			BINARY_OP(*);
			break;
		}
		case OP_DIVIDE:
		{
			BINARY_OP(/);
			break;
		}
		case OP_NEGATE:
		{
			push(-pop());
			break;
		}
		case OP_RETURN:
		{
			printValue(pop());
			printf("\n");
			return INTERPRET_OK;
		}
		}
	}

#undef READ_BYTE
#undef READ_CONSTANT
#undef BINARY_OP
}

// interpret shit and return its result
InterpretResult interpret(const char *source)
{
	Chunk chunk;
	initChunk(&chunk);

	if (!compile(source, &chunk))
	{
		freeChunk(&chunk);
		return INTERPRET_COMPILE_ERROR;
	}

	vm.chunk = &chunk;
	vm.ip = vm.chunk->code;

	InterpretResult result = run();

	freeChunk(&chunk);
	return result;
}

// push a new value onto the stack
void push(Value value)
{
	*vm.stackTop = value;
	vm.stackTop++;
}

// pop the last value off the stack
Value pop()
{
	vm.stackTop--;
	return *vm.stackTop;
}
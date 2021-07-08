#include <stdio.h>
#include <stdarg.h>

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

// display a runtime error
static void runtimeError(const char *format, ...)
{
	va_list args;
	va_start(args, format);
	vfprintf(stderr, format, args);
	va_end(args);
	fputs("\n", stderr);

	size_t instruction = vm.ip - vm.chunk->code - 1;
	int line = vm.chunk->lines[instruction];
	fprintf(stderr, "[line %d] in script\n", line);
	resetStack();
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

// return the Value at distance from top of stack without popping
static Value peek(int distance)
{
	return vm.stackTop[-1 - distance];
}

// run shit
static InterpretResult run()
{
#define READ_BYTE() (*vm.ip++)
#define READ_CONSTANT() (vm.chunk->constants.values[READ_BYTE()])
#define BINARY_OP(valueType, op)                        \
	do                                                  \
	{                                                   \
		if (!IS_NUMBER(peek(0)) || !IS_NUMBER(peek(1))) \
		{                                               \
			runtimeError("Operands must be numbers.");  \
			return INTERPRET_RUNTIME_ERROR;             \
		}                                               \
		double b = AS_NUMBER(pop());                    \
		double a = AS_NUMBER(pop());                    \
		push(valueType(a op b));                        \
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
			BINARY_OP(NUMBER_VAL, +);
			break;
		}
		case OP_SUBTRACT:
		{
			BINARY_OP(NUMBER_VAL, -);
			break;
		}
		case OP_MULTIPLY:
		{
			BINARY_OP(NUMBER_VAL, *);
			break;
		}
		case OP_DIVIDE:
		{
			BINARY_OP(NUMBER_VAL, /);
			break;
		}
		case OP_NEGATE:
		{
			if (!IS_NUMBER(peek(0)))
			{
				runtimeError("Operand must be a number.");
				return INTERPRET_RUNTIME_ERROR;
			}
			push(NUMBER_VAL(-AS_NUMBER(pop())));
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

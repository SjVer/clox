#include <stdio.h>

#include "compiler.h"
#include "common.h"
#include "debug.h"
#include "vm.h"

VM vm;

static void resetStack()
{
	vm.stackTop = vm.stack;
}

void initVM()
{
	resetStack();
}

void freeVM()
{
}

void setVMdebug(bool debug) { vm.debug = debug; }

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
		// #ifdef DEBUG_TRACE_EXECUTION
		if (vm.debug)
		{
			printf("\n\nSTACK:    ");
			// printf("          ");
			if (vm.stack[0] == *vm.stackTop)
				printf("EMPTY");
			for (Value *slot = vm.stack; slot < vm.stackTop; slot++)
			{
				printf("[");
				printValue(*slot);
				printf("]");
			}
			printf("\nINSTRUCT: ");
			disassembleInstruction(vm.chunk, (int)(vm.ip - vm.chunk->code));
			printf(">>> ");
		}
		// #endif
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

InterpretResult interpret(const char *source)
{
	compile(source);
	return INTERPRET_OK;
}

void push(Value value)
{
	*vm.stackTop = value;
	vm.stackTop++;
}

Value pop()
{
	vm.stackTop--;
	return *vm.stackTop;
}
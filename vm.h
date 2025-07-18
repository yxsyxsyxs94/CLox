#pragma once
#ifndef clox_vm_h
#define clox_vm_h

#include "chunk.h"
#include "value.h"

#define STACK_MAX 256

typedef struct {
	Chunk* chunk;
	uint8_t* ip;
	Value stack[STACK_MAX];
	Value* stackTop;
} VM;

typedef enum {
	INTERPRET_OK,
	INTERPRET_COMPILE_ERROR,
	INTERPRET_RUNTIME_ERROR
} InterpretResult;
void initVM();
void freeVM();

static void resetStack();
static void runtimeError(const char* format, ...);
void push(Value value);
Value pop();
static Value peek(int distance);

InterpretResult interpret(const char* source);
static InterpretResult run();
#endif

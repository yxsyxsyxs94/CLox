#pragma once
#ifndef clox_compiler_h
#define clox_compiler_h

#include "vm.h"

bool compile(const char* source, Chunk* chunk);
static void advance();
static void errorAtCurrent(const char* message);
static void error(const char* message);
static void errorAt(Token* token, const char* message);
static void consume(TokenType type, const char* message);
static void emitByte(uint8_t byte);
#endif
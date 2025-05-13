// compiler.h
#pragma once
#ifndef clox_compiler_h
#define clox_compiler_h

#include "vm.h"
#include "scanner.h"

namespace Compiler {
    //优先级从底到高
    typedef enum {
        PREC_NONE,
        PREC_ASSIGNMENT,  // =
        PREC_OR,          // or
        PREC_AND,         // and
        PREC_EQUALITY,    // == !=
        PREC_COMPARISON,  // < > <= >=
        PREC_TERM,        // + -
        PREC_FACTOR,      // * /
        PREC_UNARY,       // ! -
        PREC_CALL,        // . ()
        PREC_PRIMARY
    } Precedence;
    
    typedef struct {
        Scanner::Token current;
        Scanner::Token previous;
        bool hadError;
        bool panicMode;
    } Parser;

    typedef void (*ParseFn)();

    typedef struct {
        ParseFn prefix;
        ParseFn infix;
        Precedence precedence;
    } ParseRule;

    bool compile(const char* source, Chunk* chunk);
    static void advance();
    static void errorAtCurrent(const char* message);
    static void error(const char* message);
    static void errorAt(Scanner::Token* token, const char* message);
    static void consume(Scanner::TokenType type, const char* message);
    static void emitByte(uint8_t byte);
    static void emitBytes(uint8_t byte1, uint8_t byte2);
    static void endCompiler();
    static void number();
    static void unary();
    static void parsePrecedence(Precedence precedence);
    static ParseRule* getRule(Scanner::TokenType type);
    static void expression();
    static void emitReturn();
    static void emitConstant(Value value);
    static uint8_t makeConstant(Value value);
}

#endif
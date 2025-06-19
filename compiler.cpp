#include <stdio.h>
#include <stdlib.h>
#include "common.h"
#include "compiler.h"

#ifdef DEBUG_PRINT_CODE
#include "debug.h"
#endif

namespace Compiler {
	Parser parser;

	Chunk* compilingChunk;

	static Chunk* currentChunk() {
		return compilingChunk;
	}


	bool compile(const char* source, Chunk* chunk) {
		Scanner::initScanner(source);
		compilingChunk = chunk;
		parser.hadError = false;
		parser.panicMode = false;
		advance();
		expression();
		consume(Scanner::TOKEN_EOF, "Expect end of expression.");
		endCompiler();
		return !parser.hadError;
	}


	static void advance() {
		parser.previous = parser.current;

		for (;;) {
			parser.current = Scanner::scanToken();
			if (parser.current.type != Scanner::TOKEN_ERROR) break;

			errorAtCurrent(parser.current.start);
		}
	}

	static void errorAtCurrent(const char* message) {
		errorAt(&parser.current, message);
	}

	static void error(const char* message) {
		errorAt(&parser.previous, message);
	}

	static void errorAt(Scanner::Token* token, const char* message) {
		if (parser.panicMode) return;
		parser.panicMode = true;
		fprintf(stderr, "[line %d] Error", token->line);

		if (token->type == Scanner::TOKEN_EOF) {
			fprintf(stderr, " at end");
		}
		else if (token->type == Scanner::TOKEN_ERROR) {
			// Nothing.
		}
		else {
			fprintf(stderr, " at '%.*s'", token->length, token->start);
		}

		fprintf(stderr, ": %s\n", message);
		parser.hadError = true;
	}

	static void consume(Scanner::TokenType type, const char* message) {
		if (parser.current.type == type) {
			advance();
			return;
		}

		errorAtCurrent(message);
	}

	static void emitByte(uint8_t byte) {
		writeChunk(currentChunk(), byte, parser.previous.line);
	}

	static void emitBytes(uint8_t byte1, uint8_t byte2) {
		emitByte(byte1);
		emitByte(byte2);
	}

	static void endCompiler() {
		emitReturn();
#ifdef DEBUG_PRINT_CODE
		if (!parser.hadError) {
			disassembleChunk(currentChunk(), "code");
		}
#endif
	}

	static void binary() {
		//中缀计算，拿到一个operator，并没有急着emit右侧操作数
		//而是将当前operator+1作为最小优先级输入parsePrecedence
		//意味这如果里面有更高优先级，则需要优先计算，也就是此处中缀的右操作数会和更高优先级token结合
		//也就是说parsePrecedence可能里面有更高优先级，会比这个operator优先emit，从而实现比这里operator优先计算
		//本质依然是递归嵌套
		Scanner::TokenType operatorType = parser.previous.type;
		ParseRule* rule = getRule(operatorType);
		parsePrecedence((Precedence)(rule->precedence + 1));

		switch (operatorType) {
			case Scanner::TOKEN_PLUS:          emitByte(OP_ADD); break;
			case Scanner::TOKEN_MINUS:         emitByte(OP_SUBTRACT); break;
			case Scanner::TOKEN_STAR:          emitByte(OP_MULTIPLY); break;
			case Scanner::TOKEN_SLASH:         emitByte(OP_DIVIDE); break;
			default: return; // Unreachable.
		}
	}

	static void grouping() {
		expression();
		consume(Scanner::TOKEN_RIGHT_PAREN, "Expect ')' after expression.");
	}

	static void number() {
		double value = strtod(parser.previous.start, NULL);
		emitConstant(NUMBER_VAL(value));
	}

	static void unary() {
		Scanner::TokenType operatorType = parser.previous.type;

		// Compile the operand.
		parsePrecedence(PREC_UNARY);

		// Emit the operator instruction.
		switch (operatorType) {
			case Scanner::TOKEN_MINUS: emitByte(OP_NEGATE); break;
			default: return; // Unreachable.
		}
	}

	ParseRule rules[] = {
			{grouping, NULL,   PREC_NONE}, // TOKEN_LEFT_PAREN
			{NULL,     NULL,   PREC_NONE}, // TOKEN_RIGHT_PAREN
			{NULL,     NULL,   PREC_NONE}, // TOKEN_LEFT_BRACE
			{NULL,     NULL,   PREC_NONE}, // TOKEN_RIGHT_BRACE
			{NULL,     NULL,   PREC_NONE}, // TOKEN_COMMA
			{NULL,     NULL,   PREC_NONE}, // TOKEN_DOT
			{unary,    binary, PREC_TERM}, // TOKEN_MINUS
			{NULL,     binary, PREC_TERM}, // TOKEN_PLUS
			{NULL,     NULL,   PREC_NONE}, // TOKEN_SEMICOLON
			{NULL,     binary, PREC_FACTOR}, // TOKEN_SLASH
			{NULL,     binary, PREC_FACTOR}, // TOKEN_STAR
			{NULL,     NULL,   PREC_NONE}, // TOKEN_BANG
			{NULL,     NULL,   PREC_NONE}, // TOKEN_BANG_EQUAL
			{NULL,     NULL,   PREC_NONE}, // TOKEN_EQUAL
			{NULL,     NULL,   PREC_NONE}, // TOKEN_EQUAL_EQUAL
			{NULL,     NULL,   PREC_NONE}, // TOKEN_GREATER
			{NULL,     NULL,   PREC_NONE}, // TOKEN_GREATER_EQUAL
			{NULL,     NULL,   PREC_NONE}, // TOKEN_LESS
			{NULL,     NULL,   PREC_NONE}, // TOKEN_LESS_EQUAL
			{NULL,     NULL,   PREC_NONE}, // TOKEN_IDENTIFIER
			{NULL,     NULL,   PREC_NONE}, // TOKEN_STRING
			{number,   NULL,   PREC_NONE}, // TOKEN_NUMBER
			{NULL,     NULL,   PREC_NONE}, // TOKEN_AND
			{NULL,     NULL,   PREC_NONE}, // TOKEN_CLASS
			{NULL,     NULL,   PREC_NONE}, // TOKEN_ELSE
			{NULL,     NULL,   PREC_NONE}, // TOKEN_FALSE
			{NULL,     NULL,   PREC_NONE}, // TOKEN_FOR
			{NULL,     NULL,   PREC_NONE}, // TOKEN_FUN
			{NULL,     NULL,   PREC_NONE}, // TOKEN_IF
			{NULL,     NULL,   PREC_NONE}, // TOKEN_NIL
			{NULL,     NULL,   PREC_NONE}, // TOKEN_OR
			{NULL,     NULL,   PREC_NONE}, // TOKEN_PRINT
			{NULL,     NULL,   PREC_NONE}, // TOKEN_RETURN
			{NULL,     NULL,   PREC_NONE}, // TOKEN_SUPER
			{NULL,     NULL,   PREC_NONE}, // TOKEN_THIS
			{NULL,     NULL,   PREC_NONE}, // TOKEN_TRUE
			{NULL,     NULL,   PREC_NONE}, // TOKEN_VAR
			{NULL,     NULL,   PREC_NONE}, // TOKEN_WHILE
			{NULL,     NULL,   PREC_NONE}, // TOKEN_ERROR
			{NULL,     NULL,   PREC_NONE}  // TOKEN_EOF
	};

	static void parsePrecedence(Precedence precedence) {
		//往前走，生成"token ①"
		advance();
		//往前走后，回顾前一个"token②"类型，取出前一个"token②"的前缀方法
		ParseFn prefixRule = getRule(parser.previous.type)->prefix;
		if (prefixRule == NULL) {
			error("Expect expression.");
			return;
		}
		//执行上面的"token②"的前缀方法。例：常量数字，则是emit一个常量字节码
		prefixRule();
		//将"token ①"，也就是当前token的precedence和当前传入的precedence比较
		//如果当前token的precedence 大于等于，则进入while
		while (precedence <= getRule(parser.current.type)->precedence) {
			advance();
			ParseFn infixRule = getRule(parser.previous.type)->infix;
			infixRule();
		}
	}

	static ParseRule* getRule(Scanner::TokenType type) {
		return &rules[type];
	}

	static void expression() {
		parsePrecedence(PREC_ASSIGNMENT);
	}

	static void emitReturn() {
		emitByte(OP_RETURN);
	}

	static void emitConstant(Value value) {
		emitBytes(OP_CONSTANT, makeConstant(value));
	}

	static uint8_t makeConstant(Value value) {
		int constant = addConstant(currentChunk(), value);
		if (constant > UINT8_MAX) {
			error("Too many constants in one chunk.");
			return 0;
		}

		return (uint8_t)constant;
	}

}


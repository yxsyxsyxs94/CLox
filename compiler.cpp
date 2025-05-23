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
		emitConstant(value);
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
		advance();
		ParseFn prefixRule = getRule(parser.previous.type)->prefix;
		if (prefixRule == NULL) {
			error("Expect expression.");
			return;
		}

		prefixRule();

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


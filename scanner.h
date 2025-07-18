#pragma once
namespace Scanner {
	typedef enum {
		// Single-character tokens. 单字符词法
		TOKEN_LEFT_PAREN, TOKEN_RIGHT_PAREN,
		TOKEN_LEFT_BRACE, TOKEN_RIGHT_BRACE,
		TOKEN_COMMA, TOKEN_DOT, TOKEN_MINUS, TOKEN_PLUS,
		TOKEN_SEMICOLON, TOKEN_SLASH, TOKEN_STAR,
		// One or two character tokens. 一或两字符词法
		TOKEN_BANG, TOKEN_BANG_EQUAL,
		TOKEN_EQUAL, TOKEN_EQUAL_EQUAL,
		TOKEN_GREATER, TOKEN_GREATER_EQUAL,
		TOKEN_LESS, TOKEN_LESS_EQUAL,
		// Literals. 字面量
		TOKEN_IDENTIFIER, TOKEN_STRING, TOKEN_NUMBER,
		// Keywords. 关键字
		TOKEN_AND, TOKEN_CLASS, TOKEN_ELSE, TOKEN_FALSE,
		TOKEN_FOR, TOKEN_FUN, TOKEN_IF, TOKEN_NIL, TOKEN_OR,
		TOKEN_PRINT, TOKEN_RETURN, TOKEN_SUPER, TOKEN_THIS,
		TOKEN_TRUE, TOKEN_VAR, TOKEN_WHILE,

		TOKEN_ERROR, TOKEN_EOF
	} TokenType;

	typedef struct {
		TokenType type;
		const char* start;
		int length;
		int line;
	} Token;

	void initScanner(const char* source);
	Token scanToken();
	static Token errorToken(const char* message);
	static void skipWhitespace();
	static bool isAtEnd();
	static Token makeToken(TokenType type);
	static char advance();
	static bool isAlpha(char c);
	static Token identifier();
	static bool isDigit(char c);
	static Token number();
	static bool match(char expected);
	static Token string();
	static char peek();
	static char peekNext();
	static TokenType checkKeyword(int start, int length,
		const char* rest, TokenType type);
}

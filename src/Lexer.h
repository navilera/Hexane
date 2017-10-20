/*
 * Lexer.h
 *
 *  Created on: Aug 21, 2017
 *      Author: maanu
 */

#ifndef SRC_LEXER_H_
#define SRC_LEXER_H_

typedef enum {
	TOK_NOSYM = 0,
	TOK_INT,		// 1
	TOK_ID,			// 2
	TOK_STR,		// 3
	TOK_COL,		// 4
	TOK_DOT,		// 5
	TOK_LPAR,		// 6
	TOK_RPAR,		// 7
	TOK_PLUS,		// 8
	TOK_MIN,		// 9
	TOK_MUL,		// A
	TOK_DIV,		// B
	TOK_RIM,		// C
	TOK_EQU,		// D
	TOK_FUNC,		// E
	TOK_GT,			// F
	TOK_GTE,		// 10
	TOK_LT,			// 11
	TOK_LTE, 		// 12
	TOK_CEQ,		// 13
	TOK_NCEQ,		// 14
	TOK_IF,			// 15
	TOK_ELIF,		// 16
	TOK_ELSE,		// 17
	TOK_ERR,		// 18
	NumberOfSymbols
} Token_t;


#define LEX_TOK_NUM_LIMIT		1024
#define LEX_ID_TOK_LEN			512

Token_t* Lexer_GetTok(char* line);
uint64_t  Lexer_GetIntVal(uint32_t tokIndex);
char* 	  Lexer_GetIdName(uint32_t tokIndex);
uint32_t  Lexer_GetErrorPos(void);

#endif /* SRC_LEXER_H_ */

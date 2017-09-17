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
	TOK_INT,
	TOK_ID,
	TOK_STR,
	TOK_COL,
	TOK_DOT,
	TOK_LPAR,
	TOK_RPAR,
	TOK_PLUS,
	TOK_MIN,
	TOK_MUL,
	TOK_DIV,
	TOK_RIM,
	TOK_EQU,
	TOK_ERR,
	NumberOfSymbols
} Token_t;


#define LEX_TOK_NUM_LIMIT		1024
#define LEX_ID_TOK_LEN			512

Token_t* Lexer_GetTok(char* line);
uint64_t  Lexer_GetIntVal(uint32_t tokIndex);
char* 	  Lexer_GetIdName(uint32_t tokIndex);
uint32_t  Lexer_GetErrorPos(void);

#endif /* SRC_LEXER_H_ */

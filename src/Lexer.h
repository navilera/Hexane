/*
 * Lexer.h
 *
 *  Created on: Aug 21, 2017
 *      Author: maanu
 */

#ifndef SRC_LEXER_H_
#define SRC_LEXER_H_

typedef enum {
	SYM_NOSYM = 0,
	SYM_INT,
	SYM_ID,
	SYM_COL,
	SYM_DOT,
	SYM_LPAR,
	SYM_RPAR,
	SYM_PLUS,
	SYM_MIN,
	SYM_MUL,
	SYM_DIV,
	SYM_RIM,
	SYM_EQU,
	SYM_ERR,
	NumberOfSymbols
} Symbol_t;


#define LEX_SYM_NUM_LIMIT		1024
#define LEX_ID_SYM_LEN			512

Symbol_t* Lexer_GetSym(char* line);
uint64_t  Lexer_GetIntVal(uint32_t symIndex);
char* 	  Lexer_GetIdName(uint32_t symIndex);
uint32_t  Lexer_GetErrorPos(void);

#endif /* SRC_LEXER_H_ */

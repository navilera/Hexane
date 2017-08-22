/*
 * Lexer.h
 *
 *  Created on: Aug 21, 2017
 *      Author: maanu
 */

#ifndef SRC_LEXER_H_
#define SRC_LEXER_H_

typedef enum {
	SYM_INT,
	SYM_ID,
	NumberOfSymbols
} Symbol_t;


#define LEX_SYM_NUM_LIMIT		1024

Symbol_t* Lexer_GetSym(char* line);

#endif /* SRC_LEXER_H_ */

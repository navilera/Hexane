/*
 * Parser.h
 *
 *  Created on: Aug 26, 2017
 *      Author: maanu
 */

#ifndef SRC_PARSER_H_
#define SRC_PARSER_H_

typedef enum {
	BNF_NO = 0,
	BNF_program,	// 1
	BNF_assign,		// 2
	BNF_mul,		// 3
	BNF_div,		// 4
	BNF_rem,		// 5
	BNF_add,		// 6
	BNF_sub,		// 7
	BNF_var,		// 8
	BNF_const,		// 9
	BNF_str,		// A
	BNF_call,		// B
	NumberOfBnfs
} Bnf_t;

typedef struct _parser_node_t_
{
	Bnf_t type;
	struct _parser_node_t_ *child1;
	struct _parser_node_t_ *child2;
	uint64_t val;
	char* name;
} ParserNode_t;

ParserNode_t* Parser_Parse(Token_t* toklist);
void          Parser_Release(ParserNode_t* parseResult);

#endif /* SRC_PARSER_H_ */

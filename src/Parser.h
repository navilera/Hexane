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
	BNF_program,
	BNF_assign,
	BNF_mul,
	BNF_div,
	BNF_rem,
	BNF_add,
	BNF_sub,
	BNF_var,
	BNF_const,
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

ParserNode_t* Parser_Parse(Symbol_t* symlist);
void          Parser_Release(ParserNode_t* parseResult);

#endif /* SRC_PARSER_H_ */

/*
 * Parser.c
 *
 *  Created on: Aug 26, 2017
 *      Author: maanu
 */

#include "Main.h"
#include "Lexer.h"
#include "Parser.h"

/*
 * <program> ::= <statement>
 * <statement> ::= <expr>
 *               | <id> = <expr>
 * <expr> ::= <addtive_expr>
 * <multiplicative_expr> ::= <term>
 *                         | <multiplicative_expr> * <term>
 *                         | <multiplicative_expr> / <term>
 *                         | <multiplicative_expr> % <term>
 * <addtive_expr> ::= <multiplicative_expr>
 *                  | <addtive_expr> + <multiplicative_expr>
 *                  | <addtive_expr> - <multiplicative_expr>
 * <term> ::= <id>
 *          | <int>
 *          | (<expr>)
 * <id> ::= SYM_ID
 * <int> ::= SYM_INT
 */

static ParserNode_t* newNode(Bnf_t type);
static void          clearAllNode(ParserNode_t* );
static ParserNode_t* program(void);
static ParserNode_t* statement(void);
static ParserNode_t* expr(void);
static ParserNode_t* additiv_expr(void);
static ParserNode_t* mutipli_expr(void);
static ParserNode_t* term(void);

static char* getSymIdName(void);
static uint64_t getSymIntVal(void);

static Symbol_t* symStart;
static Symbol_t *symCurrent;

static bool syntaxError;

#define SYNERR_RETURN		{syntaxError = true; return NULL;}
#define CHK_SYNERR_RETURN	{if(syntaxError) {return NULL;}}

ParserNode_t* Parser_Parse(Symbol_t* symlist)
{
	syntaxError = false;
	symStart = symlist;
	symCurrent = symlist;

	ParserNode_t* parseResult = program();

	if(*symCurrent != SYM_NOSYM)
	{
		// syntax error
		return NULL;
	}

	if(syntaxError)
	{
		clearAllNode(parseResult);
		return NULL;
	}

	return parseResult;
}

void Parser_Release(ParserNode_t* parseResult)
{
	clearAllNode(parseResult);
}

static char* getSymIdName(void)
{
	int index = (int)(symCurrent - symStart);
	return Lexer_GetIdName(index);
}

static uint64_t getSymIntVal(void)
{
	int index = (int)(symCurrent - symStart);
	return Lexer_GetIntVal(index);
}

static ParserNode_t* newNode(Bnf_t type)
{
	ParserNode_t* node;

	node = (ParserNode_t*)malloc(sizeof(ParserNode_t));
	memset(node, 0, sizeof(ParserNode_t));
	node->type = type;

	return node;
}

static void clearAllNode(ParserNode_t* node)
{
	if(node != NULL)
	{
		free(node);
		clearAllNode(node->child1);
		clearAllNode(node->child2);
	}
}

// <program> ::= <statement>
static ParserNode_t* program(void)
{
	ParserNode_t* nodeProgram = newNode(BNF_program);
	nodeProgram->child1 = statement();

	return nodeProgram;
}

// <statement> ::= <expr>
//               | <id> = <expr>
static ParserNode_t* statement(void)
{
	ParserNode_t* nodeStatement;

	if(*symCurrent != SYM_ID)
	{
		nodeStatement = expr();
		CHK_SYNERR_RETURN;
	}
	else
	{
		nodeStatement = newNode(BNF_assign);
		char* variableName = getSymIdName();
		symCurrent++;
		if(*symCurrent == SYM_EQU)
		{
			ParserNode_t* nodeId = newNode(BNF_var);
			nodeId->name = variableName;
			nodeStatement->child1 = nodeId;

			symCurrent++;

			nodeStatement->child2 = expr();

			CHK_SYNERR_RETURN;
		}
		else
		{
			SYNERR_RETURN;
		}
	}

	return nodeStatement;
}

// <expr> ::= <addtive_expr>
static ParserNode_t* expr(void)
{
	ParserNode_t* nodeExpr;

	nodeExpr = additiv_expr();
	CHK_SYNERR_RETURN;

	return nodeExpr;
}

// <addtive_expr> ::= <multiplicative_expr>
//                  | <addtive_expr> + <multiplicative_expr>
//                  | <addtive_expr> - <multiplicative_expr>
static ParserNode_t* additiv_expr(void)
{
	ParserNode_t* nodeAdditiv;

	nodeAdditiv = mutipli_expr();
	CHK_SYNERR_RETURN;

	while(*symCurrent == SYM_PLUS || *symCurrent == SYM_MIN)
	{
		ParserNode_t* nodeAdd = newNode((*symCurrent == SYM_PLUS)?BNF_add:BNF_sub);
		nodeAdd->child1 = nodeAdditiv;

		symCurrent++;

		nodeAdd->child2 = mutipli_expr();
		CHK_SYNERR_RETURN;

		nodeAdditiv = nodeAdd;
	}

	return nodeAdditiv;
}

// <multiplicative_expr> ::= <term>
//                         | <multiplicative_expr> * <term>
//                         | <multiplicative_expr> / <term>
//                         | <multiplicative_expr> % <term>
static ParserNode_t* mutipli_expr(void)
{
	ParserNode_t* nodeMutipli;

	nodeMutipli = term();
	CHK_SYNERR_RETURN;

	while(*symCurrent == SYM_MUL || *symCurrent == SYM_DIV || *symCurrent == SYM_RIM)
	{
		ParserNode_t* nodeMul;
		if(*symCurrent == SYM_MUL)
		{
			nodeMul = newNode(BNF_mul);
		}
		else if(*symCurrent == SYM_DIV)
		{
			nodeMul = newNode(BNF_div);
		}
		else if(*symCurrent == SYM_RIM)
		{
			nodeMul = newNode(BNF_rem);
		}

		nodeMul->child1 = nodeMutipli;

		symCurrent++;

		nodeMul->child2 = term();
		CHK_SYNERR_RETURN;

		nodeMutipli = nodeMul;
	}

	return nodeMutipli;
}

// <term> ::= <id>
//          | <int>
//          | (<expr>)
static ParserNode_t* term(void)
{
	ParserNode_t* nodeTerm;

	if(*symCurrent == SYM_ID)
	{
		nodeTerm = newNode(BNF_var);
		nodeTerm->name = getSymIdName();
		symCurrent++;
	}
	else if(*symCurrent == SYM_INT)
	{
		nodeTerm = newNode(BNF_const);
		nodeTerm->val = getSymIntVal();
		symCurrent++;
	}
	else if(*symCurrent == SYM_LPAR)
	{
		symCurrent++;
		nodeTerm = expr();
		CHK_SYNERR_RETURN;
		if(*symCurrent == SYM_RPAR)
		{
			symCurrent++;
		}
		else
		{
			SYNERR_RETURN;
		}
	}
	else
	{
		SYNERR_RETURN;
	}

	return nodeTerm;
}


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
 * <statement> ::= <id> = <expr>
 *               | <expr>
 * <expr> ::= <str>
 * 			| <id>(<expr>)
 *          | <addtive_expr>
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
 * <str> ::= SYM_STR
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

static Token_t* symStart;
static Token_t *symCurrent;

static bool syntaxError;

#define SYNERR_RETURN		{syntaxError = true; return NULL;}
#define CHK_SYNERR_RETURN	{if(syntaxError) {return NULL;}}

ParserNode_t* Parser_Parse(Token_t* symlist)
{
	syntaxError = false;
	symStart = symlist;
	symCurrent = symlist;

	ParserNode_t* parseResult = program();

	if(*symCurrent != TOK_NOSYM)
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

// <statement> ::= <id> = <expr>
//               | <expr>
static ParserNode_t* statement(void)
{
	ParserNode_t* nodeStatement;

	if(*symCurrent == TOK_ID)
	{
		char* idName = getSymIdName();
		symCurrent++;

		if(*symCurrent == TOK_EQU)
		{
			ParserNode_t* nodeId = newNode(BNF_var);
			nodeId->name = idName;

			nodeStatement = newNode(BNF_assign);
			nodeStatement->child1 = nodeId;

			symCurrent++;

			nodeStatement->child2 = expr();
			CHK_SYNERR_RETURN;
		}
		else
		{
			// rewind
			symCurrent--;
			nodeStatement = expr();
			CHK_SYNERR_RETURN;
		}
	}
	else
	{
		nodeStatement = expr();
		CHK_SYNERR_RETURN;
	}

	return nodeStatement;
}

// <expr> ::= <str>
//          | <id>(<expr>)
//          | <addtive_expr>
static ParserNode_t* expr(void)
{
	ParserNode_t* nodeExpr;

	if(*symCurrent == TOK_STR)
	{
		nodeExpr = newNode(BNF_str);
		nodeExpr->name = getSymIdName();
		symCurrent++;
	}
//	else if(*symCurrent == SYM_ID)
//	{

//	}
	else
	{
		nodeExpr = additiv_expr();
		CHK_SYNERR_RETURN;
	}

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

	while(*symCurrent == TOK_PLUS || *symCurrent == TOK_MIN)
	{
		ParserNode_t* nodeAdd = newNode((*symCurrent == TOK_PLUS)?BNF_add:BNF_sub);
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

	while(*symCurrent == TOK_MUL || *symCurrent == TOK_DIV || *symCurrent == TOK_RIM)
	{
		ParserNode_t* nodeMul;
		if(*symCurrent == TOK_MUL)
		{
			nodeMul = newNode(BNF_mul);
		}
		else if(*symCurrent == TOK_DIV)
		{
			nodeMul = newNode(BNF_div);
		}
		else if(*symCurrent == TOK_RIM)
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

	if(*symCurrent == TOK_ID)
	{
		nodeTerm = newNode(BNF_var);
		nodeTerm->name = getSymIdName();
		symCurrent++;
	}
	else if(*symCurrent == TOK_INT)
	{
		nodeTerm = newNode(BNF_const);
		nodeTerm->val = getSymIntVal();
		symCurrent++;
	}
	else if(*symCurrent == TOK_LPAR)
	{
		symCurrent++;
		nodeTerm = expr();
		CHK_SYNERR_RETURN;
		if(*symCurrent == TOK_RPAR)
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


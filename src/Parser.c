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
 * 			| <func>(<expr>)
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
 * <id> ::= TOK_ID
 * <int> ::= TOK_INT
 * <str> ::= TOK_STR
 * <func> ::= TOK_FUNC
 */

static ParserNode_t* newNode(Bnf_t type);
static void          clearAllNode(ParserNode_t* );
static ParserNode_t* program(void);
static ParserNode_t* statement(void);
static ParserNode_t* expr(void);
static ParserNode_t* additiv_expr(void);
static ParserNode_t* mutipli_expr(void);
static ParserNode_t* term(void);

static char* getTokIdName(void);
static uint64_t getSymIntVal(void);

static Token_t* tokStart;
static Token_t *tokCurrent;

static bool syntaxError;

#define SYNERR_RETURN		{syntaxError = true; return NULL;}
#define CHK_SYNERR_RETURN	{if(syntaxError) {return NULL;}}

ParserNode_t* Parser_Parse(Token_t* toklist)
{
	syntaxError = false;
	tokStart = toklist;
	tokCurrent = toklist;

	ParserNode_t* parseResult = program();

	if(*tokCurrent != TOK_NOSYM)
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

static char* getTokIdName(void)
{
	int index = (int)(tokCurrent - tokStart);
	return Lexer_GetIdName(index);
}

static uint64_t getSymIntVal(void)
{
	int index = (int)(tokCurrent - tokStart);
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

	if(*tokCurrent == TOK_ID)
	{
		char* idName = getTokIdName();
		tokCurrent++;

		if(*tokCurrent == TOK_EQU)
		{
			ParserNode_t* nodeId = newNode(BNF_var);
			nodeId->name = idName;

			nodeStatement = newNode(BNF_assign);
			nodeStatement->child1 = nodeId;

			tokCurrent++;

			nodeStatement->child2 = expr();
			CHK_SYNERR_RETURN;
		}
		else
		{
			// rewind
			tokCurrent--;
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
//          | <func>(<expr>)
//          | <addtive_expr>
static ParserNode_t* expr(void)
{
	ParserNode_t* nodeExpr;

	if(*tokCurrent == TOK_STR)
	{
		nodeExpr = newNode(BNF_str);
		nodeExpr->name = getTokIdName();
		tokCurrent++;
	}
	else if(*tokCurrent == TOK_FUNC)
	{
		char* funcName = getTokIdName();
		tokCurrent++;
		if(*tokCurrent == TOK_LPAR)
		{
			tokCurrent++;
			ParserNode_t* subExpr = expr();
			CHK_SYNERR_RETURN;
			if(*tokCurrent == TOK_RPAR)
			{
				nodeExpr = newNode(BNF_call);
				nodeExpr->name = funcName;
				nodeExpr->child1 = subExpr;
				tokCurrent++;
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
	}
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

	while(*tokCurrent == TOK_PLUS || *tokCurrent == TOK_MIN)
	{
		ParserNode_t* nodeAdd = newNode((*tokCurrent == TOK_PLUS)?BNF_add:BNF_sub);
		nodeAdd->child1 = nodeAdditiv;

		tokCurrent++;

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

	while(*tokCurrent == TOK_MUL || *tokCurrent == TOK_DIV || *tokCurrent == TOK_RIM)
	{
		ParserNode_t* nodeMul;
		if(*tokCurrent == TOK_MUL)
		{
			nodeMul = newNode(BNF_mul);
		}
		else if(*tokCurrent == TOK_DIV)
		{
			nodeMul = newNode(BNF_div);
		}
		else if(*tokCurrent == TOK_RIM)
		{
			nodeMul = newNode(BNF_rem);
		}

		nodeMul->child1 = nodeMutipli;

		tokCurrent++;

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

	if(*tokCurrent == TOK_ID)
	{
		nodeTerm = newNode(BNF_var);
		nodeTerm->name = getTokIdName();
		tokCurrent++;
	}
	else if(*tokCurrent == TOK_INT)
	{
		nodeTerm = newNode(BNF_const);
		nodeTerm->val = getSymIntVal();
		tokCurrent++;
	}
	else if(*tokCurrent == TOK_LPAR)
	{
		tokCurrent++;
		nodeTerm = expr();
		CHK_SYNERR_RETURN;
		if(*tokCurrent == TOK_RPAR)
		{
			tokCurrent++;
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


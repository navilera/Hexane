/*
 * TestParser.c
 *
 *  Created on: Aug 27, 2017
 *      Author: maanu
 */


#include "TestMain.h"

#include "Main.h"
#include "Lexer.h"
#include "Parser.h"

REGISTER_SUITE_AUTO(ParserTest, "Parser Test")

PRE_TEST_BEGIN()
{
}
PRE_TEST_END

POST_TEST_BEGIN()
{
}
POST_TEST_END

static bool commonTestType(char* line, Bnf_t testType, int* targetDepth)
{
	Symbol_t* symlist = Lexer_GetSym(line);
	ParserNode_t* parseTree = Parser_Parse(symlist);

	if(parseTree == NULL)
	{
		printf(TESTCASEMSGINDENT "FAIL.. Syntax Error\n");
		return false;
	}

	ParserNode_t* parseTreeIndex = parseTree;
	for(int* idx = targetDepth ; *idx ; ++idx)
	{
		if(*idx == 1)
		{
			parseTreeIndex = parseTreeIndex->child1;
		}
		else if(*idx == 2)
		{
			parseTreeIndex = parseTreeIndex->child2;
		}
	}
	Bnf_t bnfType = parseTreeIndex->type;

	Parser_Release(parseTree);

	ASSERT((testType == bnfType), ASSERTMSG_INT_FAIL(testType, bnfType));
}

static bool commonTestVal(char* line, uint64_t testVal, int* targetDepth)
{
	Symbol_t* symlist = Lexer_GetSym(line);
	ParserNode_t* parseTree = Parser_Parse(symlist);

	ParserNode_t* parseTreeIndex = parseTree;
	for(int* idx = targetDepth ; *idx ; ++idx)
	{
		if(*idx == 1)
		{
			parseTreeIndex = parseTreeIndex->child1;
		}
		else if(*idx == 2)
		{
			parseTreeIndex = parseTreeIndex->child2;
		}
	}

	uint64_t val = parseTreeIndex->val;

	Parser_Release(parseTree);

	ASSERT((testVal == val), ASSERTMSG_INT_FAIL(testVal, val));
}

TESTCASE(simpleTerm, "Simple Term test")
{
	char* line = "c0FFee\n";
	int depth[2] = {1,0};
	return commonTestType(line, BNF_const, depth);
}

TESTCASE(simpleTermVal, "Simple Term value test")
{
	char* line = "c0FFee\n";
	int depth[2] = {1,0};
	return commonTestVal(line, 0xc0ffee, depth);
}

TESTCASE(simpleExprTest_add, "Simple Expression Test Add")
{
	char* line = "c0ffee + 15\n";
	int depth[2] = {1,0};
	return commonTestType(line, BNF_add, depth);
}

TESTCASE(simpleExprTest_addVal, "Simple Expression Test Add Value")
{
	char* line = "c0ffee + 15\n";
	int depth[3] = {1, 2, 0};
	return commonTestVal(line, 0x15, depth);
}

TESTCASE(complexExprTest_addsub, "Complex Expression Test AddSub")
{
	char* line = "c0ffee + 15 - 10 + 20\n";
	int depth[30] = {1, 1, 2, 0};
	return commonTestVal(line, 0x10, depth);
}

TESTCASE(complexExprTest_addmul, "Complex Expression Test AddMul")
{
	char* line = "c0ffee + 15 - 10 + 20 * 30\n";
	int depth[30] = {1, 2, 2, 0};
	return commonTestVal(line, 0x30, depth);
}

TESTCASE(complexExprTest_addmulmix01, "Complex Expression Test AddMulMix01")
{
	char* line = "c0ffee + 15 - 10 + 20 * 30+AB*30+60+CD*EFF*AD\n";
	int depth[30] = {1, 1, 1, 2, 1, 0};
	return commonTestVal(line, 0xAB, depth);
}

TESTCASE(complexExprTest_addmulmix02, "Complex Expression Test AddMulMix02")
{
	char* line = "c0ffee + 15 - 10 + 20 * 30+AB*30+60+CD*EFF*AD\n";
	int depth[30] = {1, 1, 1, 2, 1, 0};
	return commonTestVal(line, 0xAB, depth);
}

TESTCASE(complexExprTest_addmulmix03, "Complex Expression Test AddMulMix03")
{
	char* line = "c0ffee + 15 - 10 + 20 * 30+AB*30+60+CD*EFF*AD\n";
	int depth[30] = {1, 2, 1, 0};
	return commonTestType(line, BNF_mul, depth);
}


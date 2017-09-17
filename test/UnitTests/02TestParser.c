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

REGISTER_SUITE_AUTO(ParserTest, "02 Parser Test")

#if 0
PRE_TEST_BEGIN()
{
}
PRE_TEST_END

POST_TEST_BEGIN()
{
}
POST_TEST_END
#endif

static bool commonTestType(char* line, Bnf_t testType, int* targetDepth)
{
	Token_t* symlist = Lexer_GetTok(line);
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
	Bnf_t bnfType = parseTreeIndex->type;

	Parser_Release(parseTree);

	ASSERT((testType == bnfType), ASSERTMSG_INT_FAIL(testType, bnfType));
}

static bool commonTestVal(char* line, uint64_t testVal, int* targetDepth)
{
	Token_t* symlist = Lexer_GetTok(line);
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

static bool commonTestName(char* line, char* testName, int* targetDepth)
{
	Token_t* symlist = Lexer_GetTok(line);
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

	char* nodeName = parseTreeIndex->name;

	Parser_Release(parseTree);

	ASSERT(ASSERT_CMPSTR(testName, nodeName), ASSERTMSG_STR_FAIL(testName, nodeName));
}

TESTCASE(simpleTerm, "Simple Term test")
{
	char* line = "c0FFee\n";
	int depth[30] = {1,0};
	return commonTestType(line, BNF_const, depth);
}

TESTCASE(simpleTermName01, "Simple Term name test01")
{
	char* line = "$aadke\n";
	int depth[30] = {1, 0};
	return commonTestType(line, BNF_var, depth);
}


TESTCASE(simpleTermName02, "Simple Term name test02")
{
	char* line = "$aadke/200\n";
	int depth[30] = {1, 0};
	return commonTestType(line, BNF_div, depth);
}

TESTCASE(simpleTermVal, "Simple Term value test")
{
	char* line = "c0FFee\n";
	int depth[30] = {1,0};
	return commonTestVal(line, 0xc0ffee, depth);
}

TESTCASE(simpleExprTest_add, "Simple Expression Test Add")
{
	char* line = "c0ffee + 15\n";
	int depth[30] = {1,0};
	return commonTestType(line, BNF_add, depth);
}

TESTCASE(simpleExprTest_addVal, "Simple Expression Test Add Value")
{
	char* line = "c0ffee + 15\n";
	int depth[30] = {1, 2, 0};
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

TESTCASE(complexExprTest_addmulmix04, "Complex Expression Test AddMulMix04")
{
	char* line = "c0ffee + 15 - (10 + 20) * 30+AB*30+(60+CD)*EFF*AD\n";
	int depth[30] = {1, 2, 1, 1, 1, 0};
	return commonTestVal(line, 0x60, depth);
}

TESTCASE(complexExprTest_addmulmix05, "Complex Expression Test AddMulMix05")
{
	char* line = "c0ffee + 15 - (10 + 20) * 30+AB*30+(60+CD)*EFF*AD\n";
	int depth[30] = {1, 2, 1, 1, 2, 0};
	return commonTestVal(line, 0xCD, depth);
}

TESTCASE(complexExprTest_addmulmix06, "Complex Expression Test AddMulMix06")
{
	char* line = "c0ffee + 15 - (10 + 20) * 30+AB*30+(60+CD)*EFF*AD\n";
	int depth[30] = {1, 1, 1, 2, 0};
	return commonTestType(line, BNF_mul, depth);
}

TESTCASE(complexExpr_Id_Test01, "Complex Expression ID Test01")
{
	char* line = "$res = c0ffee + 15 - (10 + 20) * 30+AB*30+(60+CD)*EFF*AD\n";
	int depth[30] = {1, 1, 0};
	return commonTestType(line, BNF_var, depth);
}

TESTCASE(complexExpr_Id_Test02, "Complex Expression ID Test02")
{
	char* line = "$res = c0ffee + 15 - (10 + 20) * 30+AB*30+(60+CD)*EFF*AD\n";
	int depth[30] = {1, 1, 0};
	return commonTestName(line, "res", depth);
}

TESTCASE(complexExpr_Id_Test03, "Complex Expression ID Test03")
{
	char* line = "$res = c0ffee + 15 - (10 + $me) * 30+AB*$sx+(60+CD)*EFF*AD\n";
	int depth[30] = {1, 2, 1, 2, 2, 0};
	return commonTestName(line, "sx", depth);
}

TESTCASE(syntaxError01, "Syntax Error Test01")
{
	char* line = "$res = c0ffee + 15 - (10 + $me) * 30+AB = $sx+(60+CD)*EFF*AD\n";
	Token_t* symlist = Lexer_GetTok(line);
	ParserNode_t* parseTree = Parser_Parse(symlist);

	ASSERT((parseTree == NULL), ASSERTMSG_INT_FAIL(NULL, parseTree));
}

TESTCASE(syntaxError02, "Syntax Error Test02")
{
	char* line = "$res = c0ffee + 15 - (10 + $me) * 30+ABM + $sx+(60+CD)*EFF*AD\n";
	Token_t* symlist = Lexer_GetTok(line);
	ParserNode_t* parseTree = Parser_Parse(symlist);

	ASSERT((parseTree == NULL), ASSERTMSG_INT_FAIL(NULL, parseTree));
}

TESTCASE(stringExprTest_str01, "String test01")
{
	char* line = "\"aslg923lkd\"\n";
	int depth[30] = {1, 0};
	return commonTestType(line, BNF_str, depth);
}

TESTCASE(stringExprTest_str02, "String test01")
{
	char* line = "$ssr = \"aslg923lkd\"\n";
	int depth[30] = {1, 2, 0};
	return commonTestType(line, BNF_str, depth);
}

TESTCASE(stringExprTest_str03, "String test03")
{
	char* line = "\"aslg923lkd\"\n";
	int depth[30] = {1, 0};
	return commonTestName(line, "aslg923lkd", depth);
}

TESTCASE(stringExprTest_str04, "String test04")
{
	char* line = "$ssr = \"aslg923lkd\"\n";
	int depth[30] = {1, 2, 0};
	return commonTestName(line, "aslg923lkd", depth);
}

TESTCASE(assignErrTest01, "Assign Err")
{
	char* line = "324252=3432\n";
	Token_t* symlist = Lexer_GetTok(line);
	ParserNode_t* parseTree = Parser_Parse(symlist);

	ASSERT((parseTree == NULL), ASSERTMSG_INT_FAIL(NULL, parseTree));
}

/**
TESTCASE(functionCallTest01, "Function call test01")
{
	char* line = "dec(c0ffee)\n";
	int depth[30] = {1, 1, 0};
	return commonTestType(line, BNF_call, depth);
}

TESTCASE(functionCallTest02, "Function call test02")
{
	char* line = "$decint = dec(c0ffee)\n";
	int depth[30] = {1, 2, 0};
	return commonTestType(line, BNF_call, depth);
}
**/

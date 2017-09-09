/*
 * TestCodeGen.c
 *
 *  Created on: Aug 30, 2017
 *      Author: maanu
 */

#include "TestMain.h"

#include "Main.h"
#include "Lexer.h"
#include "Parser.h"
#include "CodeGen.h"

REGISTER_SUITE_AUTO(CodeGenTest, "03 Code Generator Test")

#define BUFLEN 1024
static char actualBuf[BUFLEN];
static char expectBuf[BUFLEN];

#define ENABLE_PUSH_STR	(NumberOfBnfs + 1)

static void serialize(uint64_t* resultlist, char* target_out, bool enablePushStr)
{
	char temp[64] = {0};
	bool nextIsId = false;
	memset(target_out, 0, BUFLEN);
	for(int i = 0 ; i < CODE_NUM_LIMIT ; ++i)
	{
		if(resultlist[i] == (uint64_t)Code_Halt)
		{
			break;
		}

		if(nextIsId == false)
		{
			sprintf(temp, "%lx,", (uint64_t)resultlist[i]);
		}
		else
		{
			sprintf(temp, "%s,", (char*)resultlist[i]);
		}

		if(resultlist[i] == (uint64_t)Code_Str || resultlist[i] == (uint64_t)Code_Ldr)
		{
			nextIsId = true;
		}
		else
		{
			if(enablePushStr && resultlist[i] == (uint64_t)Code_Push)
			{
				nextIsId = true;
			}
			else
			{
				nextIsId = false;
			}
		}
		strcat(target_out, temp);
	}
}

static bool common(char* line, uint64_t* expectlist)
{
	Symbol_t* symlist = Lexer_GetSym(line);
	ParserNode_t* parseTree = Parser_Parse(symlist);
	uint64_t* codelist = CodeGen_Compile(parseTree);

	bool enablePushStr = false;

	if(expectlist[0] == ENABLE_PUSH_STR)
	{
		enablePushStr = true;
		expectlist = &expectlist[1];
	}

	serialize(codelist, actualBuf, enablePushStr);
	serialize(expectlist, expectBuf, enablePushStr);

	ASSERT(ASSERT_CMPSTR(expectBuf, actualBuf), ASSERTMSG_STR_FAIL(expectBuf, actualBuf));
}

TESTCASE(codegen01, "Complex Expression code Gen 01")
{
	char* line = "$res = c0ffee + 15 - (10 + 20) * 30+AB*30+(60+CD)*EFF*AD\n";
	uint64_t expect[64] = {Code_Push, 0xC0FFEE, Code_Push, 0x15, Code_Add, Code_Push, 0x10, Code_Push, 0x20, Code_Add, Code_Push, 0x30, Code_Mul, Code_Sub, Code_Push, 0xab, Code_Push, 0x30,
			Code_Mul, Code_Add, Code_Push, 0x60, Code_Push, 0xcd, Code_Add, Code_Push, 0xeff, Code_Mul, Code_Push, 0xad, Code_Mul, Code_Add, Code_Str, (uint64_t)"res", Code_Pop, Code_Halt, 0};

	return common(line, expect);
}

TESTCASE(codegen02, "Complex Expression code Gen 02")
{
	char* line = "c0ffee + 15 - (10 + 20) * 30+AB*30+(60+CD)*EFF*AD\n";
	uint64_t expect[64] = {Code_Push, 0xC0FFEE, Code_Push, 0x15, Code_Add, Code_Push, 0x10, Code_Push, 0x20, Code_Add, Code_Push, 0x30, Code_Mul, Code_Sub, Code_Push, 0xab, Code_Push, 0x30,
				Code_Mul, Code_Add, Code_Push, 0x60, Code_Push, 0xcd, Code_Add, Code_Push, 0xeff, Code_Mul, Code_Push, 0xad, Code_Mul, Code_Add, Code_Halt, 0};

	return common(line, expect);
}

TESTCASE(codegen03, "Complex Expression code Gen 03")
{
	char* line = "c0ffee + 15 - (10 + $me) * 30+AB*$sx+(60+CD)*EFF*AD\n";
	uint64_t expect[64] = {Code_Push, 0xC0FFEE, Code_Push, 0x15, Code_Add, Code_Push, 0x10, Code_Ldr, (uint64_t)"me", Code_Add, Code_Push, 0x30, Code_Mul, Code_Sub, Code_Push, 0xab, Code_Ldr, (uint64_t)"sx",
				Code_Mul, Code_Add, Code_Push, 0x60, Code_Push, 0xcd, Code_Add, Code_Push, 0xeff, Code_Mul, Code_Push, 0xad, Code_Mul, Code_Add, Code_Halt, 0};

	return common(line, expect);
}

TESTCASE(codegen04, "Simple expr")
{
	char* line = "c0ffee\n";
	uint64_t expect[64] = {Code_Push, 0xC0FFEE, Code_Halt, 0};

	return common(line, expect);
}

TESTCASE(codegen05, "Simple term")
{
	char* line = "$an\n";
	uint64_t expect[64] = {Code_Ldr, (uint64_t)"an", Code_Halt, 0};

	return common(line, expect);
}

TESTCASE(codegenStr, "String")
{
	char* line = "\"asdlkgjearh4x\"\n";
	uint64_t expect[64] = {ENABLE_PUSH_STR, Code_Push, (uint64_t)"asdlkgjearh4x", Code_Halt, 0};

	return common(line, expect);
}

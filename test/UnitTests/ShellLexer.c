/*
 * ShellTest.c
 *
 *  Created on: Aug 23, 2017
 *      Author: maanu
 */

#include "TestMain.h"

#include "Main.h"
#include "Lexer.h"

static char* suiteName = "Lexer Test";

static uint32_t GetSerialize(Symbol_t* symlist)
{
	uint32_t result = 0;
	for(Symbol_t *sym = symlist; *sym != SYM_NOSYM ; ++sym)
	{
		result <<= 4;
		result |= ((uint32_t)*sym) & 0xF;
	}

	return result;
}

static bool Common(char* line, Symbol_t* expect)
{
	Symbol_t* symlist = Lexer_GetSym(line);

	uint32_t serializeActual = GetSerialize(symlist);
	uint32_t serializeExpect = GetSerialize(expect);

	ASSERT((serializeExpect == serializeActual), ASSERTMSG_INT_FAIL(serializeExpect, serializeActual));
}

static bool GetSymTest00(void)
{
	char* line = "$akaslgl 30230203 c0ffee\n";
	Symbol_t expect[4] = {SYM_ID, SYM_INT, SYM_INT, SYM_NOSYM};

	return Common(line, expect);
}

static bool GetSymTest01(void)
{
	char* line = "$akaslgl30230203 c0ffee 23849\n";
	Symbol_t expect[4] = {SYM_ID, SYM_INT, SYM_INT, SYM_NOSYM};

	return Common(line, expect);
}

static bool GetSymTest02(void)
{
	char* line = "$akaslgl30230203 c0ffee 23849 134578ae asldfpksdf 3324 $asdfl\n";
	Symbol_t expect[6] = {SYM_ID, SYM_INT, SYM_INT, SYM_INT, SYM_ERR, SYM_NOSYM};

	return Common(line, expect);
}

static bool GetSymTest03(void)
{
	char* line = "$akaslgl30230203 c0ffee 23849 134578ae $234235\n";
	Symbol_t expect[6] = {SYM_ID, SYM_INT, SYM_INT, SYM_INT, SYM_ID, SYM_NOSYM};

	return Common(line, expect);
}

static bool GetSymTest04(void)
{
	char* line = "$akaslgl30230203 c0ffee 23849 134$578ae $234235\n";
	Symbol_t expect[5] = {SYM_ID, SYM_INT, SYM_INT, SYM_ERR, SYM_NOSYM};

	return Common(line, expect);
}

static bool GetSymTest05(void)
{
	char* line = "$akaslgl30230203 c0ffee 23849 134578ae sldfpksdf 3324 $asdfl\n";
	Symbol_t expect[6] = {SYM_ID, SYM_INT, SYM_INT, SYM_INT, SYM_ERR, SYM_NOSYM};

	return Common(line, expect);
}

static bool GetSymTest06(void)
{
	char* line = "$akaslgl30230203 c0ffee 23849 134578ae sldfpksdf 3324 $asdfl\n";
	Lexer_GetSym(line);

	uint64_t getInt = Lexer_GetIntVal(2);

	ASSERT((getInt == 0x23849), ASSERTMSG_INT_FAIL(0x23849, getInt));
}

static bool GetSymTest07(void)
{
	char* line = "$akaslgl30230203 c0ffee 23849 134578ae $sldfpksdf 3324 $asdfl\n";
	Lexer_GetSym(line);

	char* getId = Lexer_GetIdName(4);

	ASSERT((strncmp(getId, "sldfpksdf", strlen("sldfpksdf")) == 0), ASSERTMSG_STR_FAIL("sldfpksdf", getId));
}

static bool GetSymTest08(void)
{
	char* line = "$akaslgl30230203 c0ffee 23849 134578ae $ 3324 $asdfl\n";
	Symbol_t expect[6] = {SYM_ID, SYM_INT, SYM_INT, SYM_INT, SYM_ERR, SYM_NOSYM};

	return Common(line, expect);
}

static void Init(TestSuite_t* suite)
{
    suite->name = suiteName;

    AddTestCase(suite, GetSymTest00, "GetSym Test01");
    AddTestCase(suite, GetSymTest01, "GetSym Test02");
    AddTestCase(suite, GetSymTest02, "GetSym Test [invalid token01]");
    AddTestCase(suite, GetSymTest03, "GetSym Test [numeric variable name]");
    AddTestCase(suite, GetSymTest04, "GetSym Test [invalid token02]");
    AddTestCase(suite, GetSymTest05, "GetSym Test [invalid token03]");
    AddTestCase(suite, GetSymTest06, "GetSym Test [get int value]");
    AddTestCase(suite, GetSymTest07, "GetSym Test [get str value]");
    AddTestCase(suite, GetSymTest08, "GetSym Test [invalid token04]");
}

REGISTER_SUITE_FUNC(ShellTest, Init)


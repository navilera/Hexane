/*
 * ShellTest.c
 *
 *  Created on: Aug 23, 2017
 *      Author: maanu
 */

#include "TestMain.h"

#include "Main.h"
#include "Lexer.h"

static char* suiteName = "01 Lexer Test";

static uint64_t GetSerialize(Symbol_t* symlist)
{
	uint64_t result = 0;
	for(Symbol_t *sym = symlist; *sym != SYM_NOSYM ; ++sym)
	{
		result <<= 4;
		result |= ((uint64_t)*sym) & 0xF;
	}

	return result;
}

static bool Common(char* line, Symbol_t* expect)
{
	Symbol_t* symlist = Lexer_GetSym(line);

	uint64_t serializeActual = GetSerialize(symlist);
	uint64_t serializeExpect = GetSerialize(expect);

	ASSERT((serializeExpect == serializeActual), ASSERTMSG_INT_FAIL(serializeExpect, serializeActual));
}

static bool GetSymTest00(void)
{
	char* line = "$akaslgl 30230203 c0ffee\n";
	Symbol_t expect[4] = {SYM_ID, SYM_INT, SYM_INT, SYM_NOSYM};

	return Common(line, expect);
}

static bool GetSymTest_term(void)
{
	char* line = "$akaslgl\n";
	Symbol_t expect[4] = {SYM_ID, SYM_NOSYM};

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

static bool GetSymTest09(void)
{
	char* line = "$akaslgl=$aoekd+400AE8+$ldske\n";
	Symbol_t expect[8] = {SYM_ID, SYM_EQU, SYM_ID, SYM_PLUS, SYM_INT, SYM_PLUS, SYM_ID, SYM_NOSYM};

	return Common(line, expect);
}

static bool GetSymTest10(void)
{
	char* line = "$akaslgl=$aoekd+400AE8+$ldske\n";
	Lexer_GetSym(line);

	uint64_t getInt = Lexer_GetIntVal(4);

	ASSERT((getInt == 0x400AE8), ASSERTMSG_INT_FAIL(0x400AE8, getInt));
}

static bool GetSymTest11(void)
{
	char* line = "$akaslgl =$aoekd + 400AE8+  $ldske\n";
	Symbol_t expect[8] = {SYM_ID, SYM_EQU, SYM_ID, SYM_PLUS, SYM_INT, SYM_PLUS, SYM_ID, SYM_NOSYM};

	return Common(line, expect);
}

static bool GetSymTest12(void)
{
	char* line = "$akaslgl30230203 c0ffee 23849 134=$578ae $234235\n";
	Symbol_t expect[8] = {SYM_ID, SYM_INT, SYM_INT, SYM_INT, SYM_EQU, SYM_ID, SYM_ID, SYM_NOSYM};

	return Common(line, expect);
}

static bool GetSymTest13(void)
{
	char* line = ":. ( +   ) $akaslgl -+30230 /203 $c0 %ffee = 23849 134=$578ae $234235\n";
	Symbol_t expect[21] = {SYM_COL, SYM_DOT, SYM_LPAR, SYM_PLUS, SYM_RPAR, SYM_ID, SYM_MIN, SYM_PLUS, SYM_INT, SYM_DIV, SYM_INT, SYM_ID, SYM_RIM, SYM_INT, SYM_EQU,
			SYM_INT, SYM_INT, SYM_EQU, SYM_ID, SYM_ID, SYM_NOSYM};

	return Common(line, expect);
}

static void Init(TestSuite_t* suite)
{
    suite->name = suiteName;

    AddTestCase(suite, GetSymTest00, "GetSym Test01");
    AddTestCase(suite, GetSymTest01, "GetSym Test02");
    AddTestCase(suite, GetSymTest_term, "only term name");
    AddTestCase(suite, GetSymTest02, "GetSym Test [invalid token01]");
    AddTestCase(suite, GetSymTest03, "GetSym Test [numeric variable name]");
    AddTestCase(suite, GetSymTest04, "GetSym Test [invalid token02]");
    AddTestCase(suite, GetSymTest05, "GetSym Test [invalid token03]");
    AddTestCase(suite, GetSymTest06, "GetSym Test [get int value]");
    AddTestCase(suite, GetSymTest07, "GetSym Test [get str value]");
    AddTestCase(suite, GetSymTest08, "GetSym Test [invalid token04]");
    AddTestCase(suite, GetSymTest09, "GetSym Test [id=id+int+id]");
    AddTestCase(suite, GetSymTest10, "GetSym Test [get int value]");
    AddTestCase(suite, GetSymTest11, "GetSym Test [mixed space]");
    AddTestCase(suite, GetSymTest12, "GetSym Test [mixed token]");
    AddTestCase(suite, GetSymTest13, "GetSym Test [complex token]");
}

REGISTER_SUITE_FUNC(LexerTest, Init)


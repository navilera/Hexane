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

#define BUFLEN 1024
static char actualBuf[BUFLEN];
static char expectBuf[BUFLEN];

static void serialize(Token_t* symlist, char* target_out)
{
	char temp[64] = {0};
	memset(target_out, 0, BUFLEN);
	for(Token_t *sym = symlist; *sym != TOK_NOSYM ; ++sym)
	{
		sprintf(temp, "%02x,", (uint32_t)((*sym) & 0xFF));
		strcat(target_out, temp);
	}
}

static bool Common(char* line, Token_t* expect)
{
	Token_t* symlist = Lexer_GetTok(line);

	serialize(symlist, actualBuf);
	serialize(expect, expectBuf);

	ASSERT(ASSERT_CMPSTR(expectBuf, actualBuf), ASSERTMSG_STR_FAIL(expectBuf, actualBuf));
}

static bool GetSymTest00(void)
{
	char* line = "$akaslgl 30230203 c0ffee\n";
	Token_t expect[4] = {TOK_ID, TOK_INT, TOK_INT, TOK_NOSYM};

	return Common(line, expect);
}

static bool GetSymTest_term(void)
{
	char* line = "$akaslgl\n";
	Token_t expect[4] = {TOK_ID, TOK_NOSYM};

	return Common(line, expect);
}

static bool GetSymTest01(void)
{
	char* line = "$akaslgl30230203 c0ffee 23849\n";
	Token_t expect[4] = {TOK_ID, TOK_INT, TOK_INT, TOK_NOSYM};

	return Common(line, expect);
}

static bool GetSymTest02(void)
{
	char* line = "$akaslgl30230203 c0ffee 23849 134578ae asldfpksdf 3324 $asdfl\n";
	Token_t expect[6] = {TOK_ID, TOK_INT, TOK_INT, TOK_INT, TOK_ERR, TOK_NOSYM};

	return Common(line, expect);
}

static bool GetSymTest03(void)
{
	char* line = "$akaslgl30230203 c0ffee 23849 134578ae $234235\n";
	Token_t expect[6] = {TOK_ID, TOK_INT, TOK_INT, TOK_INT, TOK_ID, TOK_NOSYM};

	return Common(line, expect);
}

static bool GetSymTest04(void)
{
	char* line = "$akaslgl30230203 c0ffee 23849 134$578ae $234235\n";
	Token_t expect[5] = {TOK_ID, TOK_INT, TOK_INT, TOK_ERR, TOK_NOSYM};

	return Common(line, expect);
}

static bool GetSymTest05(void)
{
	char* line = "$akaslgl30230203 c0ffee 23849 134578ae sldfpksdf 3324 $asdfl\n";
	Token_t expect[6] = {TOK_ID, TOK_INT, TOK_INT, TOK_INT, TOK_ERR, TOK_NOSYM};

	return Common(line, expect);
}

static bool GetSymTest06(void)
{
	char* line = "$akaslgl30230203 c0ffee 23849 134578ae sldfpksdf 3324 $asdfl\n";
	Lexer_GetTok(line);

	uint64_t getInt = Lexer_GetIntVal(2);

	ASSERT((getInt == 0x23849), ASSERTMSG_INT_FAIL(0x23849, getInt));
}

static bool GetSymTest07(void)
{
	char* line = "$akaslgl30230203 c0ffee 23849 134578ae $sldfpksdf 3324 $asdfl\n";
	Lexer_GetTok(line);

	char* getId = Lexer_GetIdName(4);

	ASSERT((strncmp(getId, "sldfpksdf", strlen("sldfpksdf")) == 0), ASSERTMSG_STR_FAIL("sldfpksdf", getId));
}

static bool GetSymTest08(void)
{
	char* line = "$akaslgl30230203 c0ffee 23849 134578ae $ 3324 $asdfl\n";
	Token_t expect[6] = {TOK_ID, TOK_INT, TOK_INT, TOK_INT, TOK_ERR, TOK_NOSYM};

	return Common(line, expect);
}

static bool GetSymTest09(void)
{
	char* line = "$akaslgl=$aoekd+400AE8+$ldske\n";
	Token_t expect[8] = {TOK_ID, TOK_EQU, TOK_ID, TOK_PLUS, TOK_INT, TOK_PLUS, TOK_ID, TOK_NOSYM};

	return Common(line, expect);
}

static bool GetSymTest10(void)
{
	char* line = "$akaslgl=$aoekd+400AE8+$ldske\n";
	Lexer_GetTok(line);

	uint64_t getInt = Lexer_GetIntVal(4);

	ASSERT((getInt == 0x400AE8), ASSERTMSG_INT_FAIL(0x400AE8, getInt));
}

static bool GetSymTest11(void)
{
	char* line = "$akaslgl =$aoekd + 400AE8+  $ldske\n";
	Token_t expect[8] = {TOK_ID, TOK_EQU, TOK_ID, TOK_PLUS, TOK_INT, TOK_PLUS, TOK_ID, TOK_NOSYM};

	return Common(line, expect);
}

static bool GetSymTest12(void)
{
	char* line = "$akaslgl30230203 c0ffee 23849 134=$578ae $234235\n";
	Token_t expect[8] = {TOK_ID, TOK_INT, TOK_INT, TOK_INT, TOK_EQU, TOK_ID, TOK_ID, TOK_NOSYM};

	return Common(line, expect);
}

static bool GetSymTest13(void)
{
	char* line = ":. ( +   ) $akaslgl -+30230 /203 $c0 %ffee = 23849 134=$578ae $234235\n";
	Token_t expect[21] = {TOK_COL, TOK_DOT, TOK_LPAR, TOK_PLUS, TOK_RPAR, TOK_ID, TOK_MIN, TOK_PLUS, TOK_INT, TOK_DIV, TOK_INT, TOK_ID, TOK_RIM, TOK_INT, TOK_EQU,
			TOK_INT, TOK_INT, TOK_EQU, TOK_ID, TOK_ID, TOK_NOSYM};

	return Common(line, expect);
}

static bool GetSymTest14(void)
{
	char* line = "$akaslgl30230203 c0ffee 23849 u33232\n";
	Token_t expect[16] = {TOK_ID, TOK_INT, TOK_INT, TOK_INT, TOK_NOSYM};

	return Common(line, expect);
}

static bool GetSymTest15(void)
{
	char* line = "$akaslgl30230203 c0ffee 23849u33232\n";
	Token_t expect[16] = {TOK_ID, TOK_INT, TOK_ERR, TOK_NOSYM};

	return Common(line, expect);
}

static bool GetSymTest16(void)
{
	char* line = "$akaslgl30230203 c0ffee u33232$ioi 88888\n";
	Token_t expect[16] = {TOK_ID, TOK_INT, TOK_ERR, TOK_NOSYM};

	return Common(line, expect);
}

static bool GetSymTest17(void)
{
	char* line = "$akaslgl30230203 c0ffee AAAu33232 88888\n";
	Token_t expect[16] = {TOK_ID, TOK_INT, TOK_ERR, TOK_NOSYM};

	return Common(line, expect);
}

static bool GetSymTest18(void)
{
	char* line = "$akaslgl30230203 c0ffee AAA u33232 88888\n";
	Token_t expect[16] = {TOK_ID, TOK_INT, TOK_INT, TOK_INT, TOK_INT, TOK_NOSYM};

	return Common(line, expect);
}

static bool GetSymTest19(void)
{
	char* line = "$akaslgl30230203 c0ffee $ioiu332 88888\n";
	Token_t expect[16] = {TOK_ID, TOK_INT, TOK_ID, TOK_INT, TOK_NOSYM};

	return Common(line, expect);
}

static bool GetSymTest20(void)
{
	char* line = "$akaslgl30230203 88888 u332c0ffee\n";
	Token_t expect[16] = {TOK_ID, TOK_INT, TOK_ERR, TOK_NOSYM};

	return Common(line, expect);
}

static bool GetSymTest21(void)
{
	char* line = "$akaslgl30230203 c0ffee AAA U33232 88888\n";
	Token_t expect[16] = {TOK_ID, TOK_INT, TOK_INT, TOK_INT, TOK_INT, TOK_NOSYM};

	return Common(line, expect);
}

static bool GetSymTestInvalidId(void)
{
	char* line = "$akaslgl30230203 c0ffee AAA U33232 88888 $ad$bb 999\n";
	Token_t expect[16] = {TOK_ID, TOK_INT, TOK_INT, TOK_INT, TOK_INT, TOK_ERR, TOK_NOSYM};

	return Common(line, expect);
}

static bool GetSymTestInvalidId01(void)
{
	char* line = "$akaslgl30230203 c0ffee AAA U33232 88888 $ad $bb 999\n";
	Token_t expect[16] = {TOK_ID, TOK_INT, TOK_INT, TOK_INT, TOK_INT, TOK_ID, TOK_ID, TOK_INT, TOK_NOSYM};

	return Common(line, expect);
}

static bool GetSymQuote(void)
{
	char* line = "$akaslgl30230203 c0ffee AAA U33232 \"c0ffee\" 88888\n";
	Token_t expect[16] = {TOK_ID, TOK_INT, TOK_INT, TOK_INT, TOK_STR, TOK_INT, TOK_NOSYM};

	return Common(line, expect);
}

static bool GetSymQuoteName(void)
{
	char* line = "$akaslgl30230203 c0ffee AAA U33232 \"asksl3kks982ls\" 88888\n";
	Lexer_GetTok(line);

	char* getName = Lexer_GetIdName(4);

	ASSERT(ASSERT_CMPSTR("asksl3kks982ls", getName), ASSERTMSG_STR_FAIL("asksl3kks982ls", getName));
}

static bool GetSymQuoteErr01(void)
{
	char* line = "$akaslgl30230203 c0ffee AAA U33232\"c0ffee\" 88888\n";
	Token_t expect[16] = {TOK_ID, TOK_INT, TOK_INT, TOK_ERR, TOK_NOSYM};

	return Common(line, expect);
}

static bool GetSymQuoteErr02(void)
{
	char* line = "$akaslgl30230203 c0ffee AAA $adg\"c0ffee\" 88888\n";
	Token_t expect[16] = {TOK_ID, TOK_INT, TOK_INT, TOK_ERR, TOK_NOSYM};

	return Common(line, expect);
}

static bool GetAssingErr(void)
{
	char* line = "3234=5542\n";
	Token_t expect[16] = {TOK_INT, TOK_EQU, TOK_INT, TOK_NOSYM};

	return Common(line, expect);
}

static bool FunctionCall(void)
{
	char* line = "dec(c0ffee)\n";
	Token_t expect[16] = {TOK_FUNC, TOK_LPAR, TOK_INT, TOK_RPAR, TOK_NOSYM};

	return Common(line, expect);
}

static bool FunctionCall01(void)
{
	char* line = "dec  ( c0ffee  )\n";
	Token_t expect[16] = {TOK_FUNC, TOK_LPAR, TOK_INT, TOK_RPAR, TOK_NOSYM};

	return Common(line, expect);
}

static bool FunctionCall02(void)
{
	char* line = "$dec = c0ffee  ( c0ffee  )\n";
	Token_t expect[16] = {TOK_ID, TOK_EQU, TOK_FUNC, TOK_LPAR, TOK_INT, TOK_RPAR, TOK_NOSYM};

	return Common(line, expect);
}

static bool FunctionCall03_err01(void)
{
	char* line = "$dec = 9934nek  ( c0ffee  )\n";
	Token_t expect[16] = {TOK_ID, TOK_EQU, TOK_ERR, TOK_NOSYM};

	return Common(line, expect);
}

static bool GetFunctionName(void)
{
	char* line = "$dec = c0ffee(c0ffee)\n";
	Lexer_GetTok(line);

	char* getName = Lexer_GetIdName(2);

	ASSERT(ASSERT_CMPSTR("c0ffee", getName), ASSERTMSG_STR_FAIL("c0ffee", getName));
}

static bool ConditionTokenGT(void)
{
	char* line = "$dec = 3 > 5 + 4 >= $n =>u332\n";
	Token_t expect[16] = {TOK_ID, TOK_EQU, TOK_INT, TOK_GT, TOK_INT, TOK_PLUS, TOK_INT, TOK_GTE, TOK_ID, TOK_GTE, TOK_INT, TOK_NOSYM};

	return Common(line, expect);
}

static bool ConditionTokenLT(void)
{
	char* line = "u332 <534 <= 902 =< $kos\n";
	Token_t expect[16] = {TOK_INT, TOK_LT, TOK_INT, TOK_LTE, TOK_INT, TOK_LTE, TOK_ID, TOK_NOSYM};

	return Common(line, expect);
}

static bool ConditionTokenEQ(void)
{
	char* line = "$kos== 9920 != $cmmso =! 849\n";
	Token_t expect[16] = {TOK_ID, TOK_CEQ, TOK_INT, TOK_NCEQ, TOK_ID, TOK_NCEQ, TOK_INT, TOK_NOSYM};

	return Common(line, expect);
}

static bool ConditionTokenEQErr(void)
{
	char* line = "$kos== 9920 !< $cmmso =! 849\n";
	Token_t expect[16] = {TOK_ID, TOK_CEQ, TOK_INT, TOK_ERR, TOK_NOSYM};

	return Common(line, expect);
}

static bool ReservedKeywordErr01(void)
{
	char* line = "$kos== 9920 + if( $cmmso) =! 849\n";
	Token_t expect[16] = {TOK_ID, TOK_CEQ, TOK_INT, TOK_PLUS, TOK_ERR, TOK_NOSYM};

	return Common(line, expect);
}

static bool ReservedKeywordErr02(void)
{
	char* line = "$kos== 9920 + elif( $cmmso) =! 849\n";
	Token_t expect[16] = {TOK_ID, TOK_CEQ, TOK_INT, TOK_PLUS, TOK_ERR, TOK_NOSYM};

	return Common(line, expect);
}

static bool ReservedKeywordErr03(void)
{
	char* line = "$kos== 9920 + else( $cmmso) =! 849\n";
	Token_t expect[16] = {TOK_ID, TOK_CEQ, TOK_INT, TOK_PLUS, TOK_ERR, TOK_NOSYM};

	return Common(line, expect);
}

static bool ReservedKeywordIfElifElse01(void)
{
	char* line = "$kos== 9920 if != $cmmso =! 849\n";
	Token_t expect[16] = {TOK_ID, TOK_CEQ, TOK_INT, TOK_IF, TOK_NCEQ, TOK_ID, TOK_NCEQ, TOK_INT, TOK_NOSYM};

	return Common(line, expect);
}

static bool ReservedKeywordIfElifElse02(void)
{
	char* line = "$kos== 9920 elif != $cmmso =! 849\n";
	Token_t expect[16] = {TOK_ID, TOK_CEQ, TOK_INT, TOK_ELIF, TOK_NCEQ, TOK_ID, TOK_NCEQ, TOK_INT, TOK_NOSYM};

	return Common(line, expect);
}

static bool ReservedKeywordIfElifElse03(void)
{
	char* line = "$kos== 9920 else != $cmmso =! 849\n";
	Token_t expect[16] = {TOK_ID, TOK_CEQ, TOK_INT, TOK_ELSE, TOK_NCEQ, TOK_ID, TOK_NCEQ, TOK_INT, TOK_NOSYM};

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
    AddTestCase(suite, GetSymTest14, "GetSym Test [Dec01]");
    AddTestCase(suite, GetSymTest15, "GetSym Test [Dec err00]");
    AddTestCase(suite, GetSymTest16, "GetSym Test [Dec err01]");
    AddTestCase(suite, GetSymTest17, "GetSym Test [Dec err02]");
    AddTestCase(suite, GetSymTest18, "GetSym Test [Dec02]");
    AddTestCase(suite, GetSymTest19, "GetSym Test [Dec03]");
    AddTestCase(suite, GetSymTest20, "GetSym Test [Dec err03]");
    AddTestCase(suite, GetSymTest21, "GetSym Test [Dec04]");
    AddTestCase(suite, GetSymTestInvalidId, "GetSym Test [Invalid ID]");
    AddTestCase(suite, GetSymTestInvalidId01, "GetSym Test [Invalid ID01]");
    AddTestCase(suite, GetSymQuote, "GetSym Test [Quete]");
    AddTestCase(suite, GetSymQuoteName, "GetSym Test [QueteName]");
    AddTestCase(suite, GetSymQuoteErr01, "GetSym Test [QueteErr01]");
    AddTestCase(suite, GetSymQuoteErr02, "GetSym Test [QueteErr02]");
    AddTestCase(suite, GetAssingErr, "GetSym Test [Assign Error]");
    AddTestCase(suite, FunctionCall, "Function call expression");
    AddTestCase(suite, FunctionCall01, "Function call with spaces");
    AddTestCase(suite, FunctionCall02, "Function call weird name but Okay");
    AddTestCase(suite, FunctionCall03_err01, "Function call name error");
    AddTestCase(suite, GetFunctionName, "Function call get function name");
    AddTestCase(suite, ConditionTokenGT, "Condition token GT and GTE");
    AddTestCase(suite, ConditionTokenLT, "Condition token LT and LTE");
    AddTestCase(suite, ConditionTokenEQ, "Condition token CEQ and NCEQ");
    AddTestCase(suite, ConditionTokenEQErr, "Condition token NCEQ error");
    AddTestCase(suite, ReservedKeywordErr01, "Reserved keyword Err01 - if");
    AddTestCase(suite, ReservedKeywordErr02, "Reserved keyword Err02 - elif");
    AddTestCase(suite, ReservedKeywordErr03, "Reserved keyword Err03 - else");
    AddTestCase(suite, ReservedKeywordIfElifElse01, "Reserved keyword - if");
    AddTestCase(suite, ReservedKeywordIfElifElse02, "Reserved keyword - elif");
    AddTestCase(suite, ReservedKeywordIfElifElse03, "Reserved keyword - else");
}

REGISTER_SUITE_FUNC(LexerTest, Init)


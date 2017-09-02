/*
 * 04TestVm.c
 *
 *  Created on: Sep 2, 2017
 *      Author: maanu
 */



#include "TestMain.h"

#include "Main.h"
#include "Lexer.h"
#include "Parser.h"
#include "CodeGen.h"
#include "VirtualMachine.h"

REGISTER_SUITE_AUTO(VMTest, "Virtual Machine Test")


static uint64_t* common(char* line, uint64_t expect)
{
	Symbol_t* symlist = Lexer_GetSym(line);
	ParserNode_t* parseTree = Parser_Parse(symlist);
	uint64_t* codelist = CodeGen_Compile(parseTree);
	uint64_t* sp = Vm_Run(codelist);

	if(Vm_IsStackEmpty(sp))
	{
		return NULL;
	}

	return sp;
}

static bool testVal(char* line, uint64_t expect)
{
	uint64_t* sp = common(line, expect);
	ASSERT((expect == Vm_GetStackValue(sp)), ASSERTMSG_INT_FAIL(expect, Vm_GetStackValue(sp)));
}

static bool assignVarStackEmpty(char* line)
{
	Symbol_t* symlist = Lexer_GetSym(line);
	ParserNode_t* parseTree = Parser_Parse(symlist);
	uint64_t* codelist = CodeGen_Compile(parseTree);
	uint64_t* sp = Vm_Run(codelist);

	ASSERT((Vm_IsStackEmpty(sp) == true), ASSERTMSG_INT_FAIL(true, Vm_IsStackEmpty(sp)));
}

static bool expectErr(char* line)
{
	Symbol_t* symlist = Lexer_GetSym(line);
	ParserNode_t* parseTree = Parser_Parse(symlist);
	uint64_t* codelist = CodeGen_Compile(parseTree);
	uint64_t* sp = Vm_Run(codelist);

	ASSERT((sp == NULL), ASSERTMSG_INT_FAIL((uint64_t)NULL, sp));
}

TESTCASE(Vm01, "VM simple int")
{
	char* line = "c0FFee\n";
	return testVal(line, 0xc0ffee);
}

TESTCASE(Vm02_expectEmpty, "VM Expect Empty Stack")
{
	char* line = "$adkels= afdde87\n";
	return assignVarStackEmpty(line);
}

TESTCASE(Vm03_getSymVal, "VM retrieve symbol value")
{
	char* line = "$adkels\n";
	return testVal(line, 0xafdde87);
}

TESTCASE(Vm04_complexExpr, "VM complex expr")
{
	char* line = "c0ffee + 15 - (10 + 20) * 30+AB*30+(60+CD)*EFF*AD\n";
	return testVal(line, 0xcab72aa);
}

TESTCASE(Vm05_assignTest, "VM assign variable test")
{
	char* line = "$aadokk = c0ffee + 15 - (10 + 20) * 30+AB*30+(60+CD)*EFF*AD\n";
	return assignVarStackEmpty(line);
}

TESTCASE(Vm06_loadVar, "VM load variable test01")
{
	char* line = "c0ffee + 15 - (10 + $aadokk) * 30+AB*$adkels+(60+CD)*EFF*AD\n";
	return testVal(line, 0x504197ce7);
}

TESTCASE(Vm07_loadErr01, "VM retrieve symbol error value")
{
	char* line = "$adke343\n";
	return expectErr(line);
}

TESTCASE(Vm08_loadErr02, "VM load variable test02")
{
	char* line = "c0ffee + 15 - (10 + $aaddokk) * 30+AB*$adkels+(60+CD)*EFF*AD\n";
	return expectErr(line);
}

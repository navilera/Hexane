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

REGISTER_SUITE_AUTO(VMTest, "04 Virtual Machine Test")


static VmStack_t* common(char* line)
{
	Symbol_t* symlist = Lexer_GetSym(line);
	ParserNode_t* parseTree = Parser_Parse(symlist);
	CodegenList_t* codelist = CodeGen_Compile(parseTree);
	VmStack_t* sp = Vm_Run(codelist);

	return sp;
}

static bool testVal(char* line, char* expect)
{
	VmStack_t* sp = common(line);
	ASSERT(ASSERT_CMPSTR(expect,Vm_GetStackValue(sp)), ASSERTMSG_STR_FAIL(expect, Vm_GetStackValue(sp)));
}

static bool assignVarStackEmpty(char* line)
{
	Symbol_t* symlist = Lexer_GetSym(line);
	ParserNode_t* parseTree = Parser_Parse(symlist);
	CodegenList_t* codelist = CodeGen_Compile(parseTree);
	VmStack_t* sp = Vm_Run(codelist);

	ASSERT((Vm_IsStackEmpty(sp) == true), ASSERTMSG_INT_FAIL(true, Vm_IsStackEmpty(sp)));
}

static bool expectErr(char* line)
{
	Symbol_t* symlist = Lexer_GetSym(line);
	ParserNode_t* parseTree = Parser_Parse(symlist);
	CodegenList_t* codelist = CodeGen_Compile(parseTree);
	VmStack_t* sp = Vm_Run(codelist);

	ASSERT((sp == NULL), ASSERTMSG_INT_FAIL((uint64_t)NULL, sp));
}

TESTCASE(Vm01, "VM simple int")
{
	char* line = "c0FFee\n";
	return testVal(line, "c0ffee");
}

TESTCASE(Vm02_expectEmpty, "VM Expect Empty Stack")
{
	char* line = "$adkels= afdde87\n";
	return assignVarStackEmpty(line);
}

TESTCASE(Vm02_readVal, "VM read val")
{
	char* line = "$adkels= afdde876\n";
	char* nextline = "$adkels\n";
	common(line);
	return testVal(nextline, "afdde876");
}

TESTCASE(Vm03_getSymVal, "VM retrieve symbol value")
{
	char* line = "$adkels\n";
	return testVal(line, "afdde876");
}

TESTCASE(Vm04_complexExpr, "VM complex expr")
{
	char* line = "c0ffee + 15 - (10 + 20) * 30+AB*30+(60+CD)*EFF*AD\n";
	return testVal(line, "cab72aa");
}

TESTCASE(Vm05_assignTest, "VM assign variable test")
{
	char* line = "$aadokk = c0ffee + 15 - (10 + 20) * 30+AB*30+(60+CD)*EFF*AD\n";
	return assignVarStackEmpty(line);
}

TESTCASE(Vm06_loadVar, "VM load variable test01")
{
	char* line = "c0ffee + 15 - (10 + $aadokk) * 30+AB*$adkels+(60+CD)*EFF*AD\n";
	return testVal(line, "7325c01f8c");
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

TESTCASE(Vm09_string, "VM string test01")
{
	char* line = "\"dfg3ds34t\"\n";
	return testVal(line, "\"dfg3ds34t\"");
}

TESTCASE(Vm10_string02, "VM string test02")
{
	char* line = "$adkels= \"dfg3ds34t\"\n";
	char* nextline = "$adkels\n";
	common(line);
	return testVal(nextline, "\"dfg3ds34t\"");
}

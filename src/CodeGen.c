/*
 * CodeGen.c
 *
 *  Created on: Aug 29, 2017
 *      Author: maanu
 */

#include "Main.h"
#include "Lexer.h"
#include "Parser.h"
#include "CodeGen.h"

static CodegenList_t lineCodeBuff[CODE_NUM_LIMIT];
static int lineCodeBuffIndex;

static void compiler(ParserNode_t* parseNode);
static void addObjCode(ItmdCode_t c, CodeType_t type);
static void addObjVal(uint64_t v, CodeType_t type);
static void addObjName(char* n, CodeType_t type);

CodegenList_t* CodeGen_Compile(ParserNode_t* parseTree)
{
	lineCodeBuffIndex = 0;
	compiler(parseTree);
	return lineCodeBuff;
}

static void compiler(ParserNode_t* parseNode)
{
	switch(parseNode->type)
	{
	case BNF_NO: break;
	case BNF_program:
		lineCodeBuffIndex = 0;
		compiler(parseNode->child1);
		addObjCode(Code_Halt, CodeType_Instr);
		break;
	case BNF_assign:
		compiler(parseNode->child2);
		addObjCode(Code_Str, CodeType_Instr);
		addObjName(parseNode->child1->name, CodeType_Addr);
		addObjCode(Code_Pop, CodeType_Instr);
		break;
	case BNF_mul:
		compiler(parseNode->child1);
		compiler(parseNode->child2);
		addObjCode(Code_Mul, CodeType_Instr);
		break;
	case BNF_div:
		compiler(parseNode->child1);
		compiler(parseNode->child2);
		addObjCode(Code_Div, CodeType_Instr);
		break;
	case BNF_rem:
		compiler(parseNode->child1);
		compiler(parseNode->child2);
		addObjCode(Code_Mod, CodeType_Instr);
		break;
	case BNF_add:
		compiler(parseNode->child1);
		compiler(parseNode->child2);
		addObjCode(Code_Add, CodeType_Instr);
		break;
	case BNF_sub:
		compiler(parseNode->child1);
		compiler(parseNode->child2);
		addObjCode(Code_Sub, CodeType_Instr);
		break;
	case BNF_var:
		addObjCode(Code_Ldr, CodeType_Instr);
		addObjName(parseNode->name, CodeType_Addr);
		break;
	case BNF_str:
		addObjCode(Code_Push, CodeType_Instr);
		addObjName(parseNode->name, CodeType_Str);
		break;
	case BNF_const:
		addObjCode(Code_Push, CodeType_Instr);
		addObjVal(parseNode->val, CodeType_Int);
		break;
	case BNF_call:
		compiler(parseNode->child1);
		addObjCode(Code_Jmp, CodeType_Instr);
		addObjName(parseNode->name, CodeType_Addr);
		break;
	case NumberOfBnfs: break;
	}
}

static void addObjCode(ItmdCode_t c, CodeType_t type)
{
	lineCodeBuff[lineCodeBuffIndex].val = (uint64_t)c;
	lineCodeBuff[lineCodeBuffIndex].type = type;
	lineCodeBuffIndex++;
}

static void addObjVal(uint64_t v, CodeType_t type)
{
	lineCodeBuff[lineCodeBuffIndex].val = v;
	lineCodeBuff[lineCodeBuffIndex].type = type;
	lineCodeBuffIndex++;
}

static void addObjName(char* n, CodeType_t type)
{
	lineCodeBuff[lineCodeBuffIndex].val = (uint64_t)n;
	lineCodeBuff[lineCodeBuffIndex].type = type;
	lineCodeBuffIndex++;
}

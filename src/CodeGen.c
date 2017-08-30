/*
 * CodeGen.c
 *
 *  Created on: Aug 29, 2017
 *      Author: maanu
 */

#include "CodeGen.h"

static uint64_t lineCodeBuff[CODE_NUM_LIMIT];
static int lineCodeBuffIndex;

static void compiler(ParserNode_t* parseNode);
static void addObjCode(ItmdCode_t c);
static void addObjVal(uint64_t v);
static void addObjName(char* n);

ItmdCode_t* CodeGen_Compile(ParserNode_t* parseTree)
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
		addObjCode(Code_Halt);
		break;
	case BNF_assign:
		compiler(parseNode->child2);
		addObjCode(Code_Str);
		addObjName(parseNode->child1->name);
		break;
	case BNF_mul:
		compiler(parseNode->child1);
		compiler(parseNode->child2);
		addObjCode(Code_Mul);
		break;
	case BNF_div:
		compiler(parseNode->child1);
		compiler(parseNode->child2);
		addObjCode(Code_Div);
		break;
	case BNF_rem:
		compiler(parseNode->child1);
		compiler(parseNode->child2);
		addObjCode(Code_Mod);
		break;
	case BNF_add:
		compiler(parseNode->child1);
		compiler(parseNode->child2);
		addObjCode(Code_Add);
		break;
	case BNF_sub:
		compiler(parseNode->child1);
		compiler(parseNode->child2);
		addObjCode(Code_Sub);
		break;
	case BNF_var:
		addObjCode(Code_Ldr);
		addObjName(parseNode->name);
		break;
	case BNF_const:
		addObjCode(Code_Mov);
		addObjVal(parseNode->val);
		break;
	case NumberOfBnfs: break;
	}
}

static void addObjCode(ItmdCode_t c)
{
	lineCodeBuff[lineCodeBuffIndex++] = (uint64_t)c;
}

static void addObjVal(uint64_t v)
{
	lineCodeBuff[lineCodeBuffIndex++] = v;
}

static void addObjName(char* n)
{
	lineCodeBuff[lineCodeBuffIndex++] = (uint64_t)n;
}

/*
 * VirtualMachine.c
 *
 *  Created on: Sep 1, 2017
 *      Author: maanu
 */

#include "Main.h"
#include "Lexer.h"
#include "Parser.h"
#include "CodeGen.h"
#include "VirtualMachine.h"
#include "BuiltInFunction.h"

static VmSymbolTable_t symbolTable[VM_SYMTBLNUM] = {0};
static VmStack_t stack[VM_STACKSIZE];

static char errorMessage[VM_ERRSTRLEN];
static char resultStr[VM_RETSTRLEN];

static bool storeSymbolTable(char* name, VmStack_t spval);
static int loadSymbolTable(char* name, VmStack_t* spval_out);
static void errorMsg(char* errMsg);
static int builtInFunction(char* funcName, VmStack_t* currentSp_in_out);


VmStack_t* Vm_Run(CodegenList_t* code)
{
	memset(stack, 0, VM_STACKSIZE);
	memset(errorMessage, 0, VM_ERRSTRLEN);

	VmStack_t* sp = stack;
	CodegenList_t* pc = code;

	while(true)
	{
		ItmdCode_t instr = (ItmdCode_t)pc->val;
		pc++;

		switch(instr)
		{
		case Code_Str:
			if(!storeSymbolTable((char*)pc->val, sp[-1]))
			{
				errorMsg(VM_ERR_not_enough_symbol_table);
				return NULL;	// error
			}
			pc++;
			break;
		case Code_Ldr:
			if(loadSymbolTable((char*)pc->val, sp) == -1)
			{
				errorMsg(VM_ERR_Invalid_variable);
				return NULL;	// error
			}
			sp++;
			pc++;
			break;
		case Code_Push:
			sp->val = pc->val;
			sp->type = (pc->type == CodeType_Int) ? VmStackType_Int : VmStackType_Str;
			sp++;
			pc++;
			break;
		case Code_Pop:
			--sp;
			break;
		case Code_Add:
			sp[-2].val = sp[-2].val + sp[-1].val; --sp;
			break;
		case Code_Sub:
			sp[-2].val = sp[-2].val - sp[-1].val; --sp;
			break;
		case Code_Mul:
			sp[-2].val = sp[-2].val * sp[-1].val; --sp;
			break;
		case Code_Div:
			sp[-2].val = sp[-2].val / sp[-1].val; --sp;
			break;
		case Code_Mod:
			sp[-2].val = sp[-2].val % sp[-1].val; --sp;
			break;
		case Code_Jmp:
		{
			int popCount = builtInFunction((char*)pc->val, sp);
			if(popCount < 0)
			{
				VmStack_t funcAddr;
				if(loadSymbolTable((char*)pc->val, &funcAddr) == -1)
				{
					errorMsg(VM_ERR_undefined_function);
					return NULL;	// error
				}
				pc = (CodegenList_t*)funcAddr.val;
			}
			else
			{
				sp -= popCount;
			}
		}
			pc++;
			break;
		case Code_Halt:
			return sp;
			break;
		case Code_No:
		case NumberOfCodes:
		default:
			errorMsg(VM_ERR_critical_error);
			return NULL;	// error
		}
	}

	return NULL;
}

bool Vm_IsStackEmpty(VmStack_t* sp)
{
	return (sp == stack);
}

char* Vm_GetStackValue(VmStack_t* sp)
{
	VmStack_t ret = *(sp - 1);
	if(ret.type == VmStackType_Int)
	{
		sprintf(resultStr, "%lx", ret.val);
	}
	else if(ret.type == VmStackType_Str)
	{
		sprintf(resultStr, "\"%s\"", (char*)ret.val);
	}
	return resultStr;
}

char* Vm_GetErrorMsg(void)
{
	return errorMessage;
}

static void errorMsg(char* errMsg)
{
	strncpy(errorMessage, errMsg, strlen(errMsg));
}

static bool storeSymbolTable(char* name, VmStack_t spval)
{
	VmStack_t temp;
	int index = 0;
	if((index = loadSymbolTable(name, &temp)) != -1)
	{
		if(symbolTable[index].val.type == VmStackType_Str)
		{
			if(symbolTable[index].val.val != 0)
			{
				free((void*)symbolTable[index].val.val);
			}
		}

		symbolTable[index].val.type = spval.type;

		if(spval.type == VmStackType_Str)
		{
			symbolTable[index].val.val = (uint64_t)(char*)malloc(strlen((char*)spval.val) + 1);
			memset((void*)(symbolTable[index].val.val), 0, strlen((char*)spval.val) + 1);
			strncpy((char*)(symbolTable[index].val.val), (char*)(spval.val), strlen((char*)spval.val));
		}
		else
		{
			symbolTable[index].val.val = spval.val;
		}
		return true;
	}

	for(int i = 0 ; i < VM_SYMTBLNUM ; ++i)
	{
		if(symbolTable[i].name == NULL)
		{
			symbolTable[i].name = (char*)malloc(strlen(name) + 1);
			memset(symbolTable[i].name, 0, strlen(name) + 1);
			strncpy(symbolTable[i].name, name, strlen(name));

			symbolTable[i].val.type = spval.type;

			if(spval.type == VmStackType_Str)
			{
				symbolTable[i].val.val = (uint64_t)(char*)malloc(strlen((char*)spval.val) + 1);
				memset((void*)(symbolTable[i].val.val), 0, strlen((char*)spval.val) + 1);
				strncpy((char*)(symbolTable[i].val.val), (char*)(spval.val), strlen((char*)spval.val));
			}
			else
			{
				symbolTable[i].val.val = spval.val;
			}
			return true;
		}
	}
	return false;
}

static int loadSymbolTable(char* name, VmStack_t* spval_out)
{
	for(int i = 0 ; i < VM_SYMTBLNUM ; ++i)
	{
		if(symbolTable[i].name != NULL)
		{
			if(STR_CMP(symbolTable[i].name, name))
			{
				*spval_out = symbolTable[i].val;
				return i;
			}
		}
	}

	return (-1);
}

static int builtInFunction(char* funcName, VmStack_t* currentSp_in_out)
{
	int popSpNum = 0;

	switch(BuiltInFunc_Check(funcName))
	{
	case BLTINFUN_dec:
	{
		char* decStr = BuiltInFunc_Dec(currentSp_in_out[-1].val);
		currentSp_in_out[-1].val = (uint64_t)decStr;
		currentSp_in_out[-1].type = VmStackType_Str;
		popSpNum = 0;
	}
		break;
	case BLTINFUN_No:
	default:
		return -1;
	}

	return popSpNum;
}


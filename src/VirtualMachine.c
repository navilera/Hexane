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

VmSymbolTable_t symbolTable[VM_SYMTBLNUM] = {0};
uint64_t stack[VM_STACKSIZE];

static char errorMessage[VM_ERRSTRLEN];

static bool storeSymbolTable(char* name, uint64_t val);
static uint64_t loadSymbolTable(char* name);
static bool isExistSymbolTable(char* name);
static void errorMsg(char* errMsg);


uint64_t* Vm_Run(uint64_t* code)
{
	memset(stack, 0, VM_STACKSIZE);
	memset(errorMessage, 0, VM_ERRSTRLEN);

	uint64_t* sp = stack;
	uint64_t* pc = code;

	while(true)
	{
		switch((ItmdCode_t)*pc++)
		{
		case Code_Str:
			if(!storeSymbolTable((char*)*pc++, sp[-1]))
			{
				errorMsg(VM_ERR_not_enough_symbol_table);
				return NULL;	// error
			}
			break;
		case Code_Ldr:
			if(isExistSymbolTable((char*)*pc))
			{
				*sp++ = loadSymbolTable((char*)*pc++);
			}
			else
			{
				errorMsg(VM_ERR_Invalid_variable);
				return NULL;	// error
			}
			break;
		case Code_Push:
			*sp++ = *pc++;
			break;
		case Code_Pop:
			--sp;
			break;
		case Code_Add:
			sp[-2] = sp[-2] + sp[-1]; --sp;
			break;
		case Code_Sub:
			sp[-2] = sp[-2] - sp[-1]; --sp;
			break;
		case Code_Mul:
			sp[-2] = sp[-2] * sp[-1]; --sp;
			break;
		case Code_Div:
			sp[-2] = sp[-2] / sp[-1]; --sp;
			break;
		case Code_Mod:
			sp[-2] = sp[-2] % sp[-1]; --sp;
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

bool Vm_IsStackEmpty(uint64_t* sp)
{
	return (sp == stack);
}

uint64_t Vm_GetStackValue(uint64_t* sp)
{
	return *(sp - 1);
}

char* Vm_GetErrorMsg(void)
{
	return errorMessage;
}

static void errorMsg(char* errMsg)
{
	strncpy(errorMessage, errMsg, strlen(errMsg));
}

static bool storeSymbolTable(char* name, uint64_t val)
{
	for(int i = 0 ; i < VM_SYMTBLNUM ; ++i)
	{
		if(symbolTable[i].name == NULL)
		{
			symbolTable[i].name = (char*)malloc(sizeof(name));
			strncpy(symbolTable[i].name, name, strlen(name));
			symbolTable[i].val = val;
			return true;
		}
	}
	return false;
}

static uint64_t loadSymbolTable(char* name)
{
	for(int i = 0 ; i < VM_SYMTBLNUM ; ++i)
	{
		if(symbolTable[i].name != NULL)
		{
			if((strncmp(symbolTable[i].name, name, strlen(name)) == 0) && (strlen(symbolTable[i].name) == strlen(name)))
			{
				return symbolTable[i].val;
			}
		}
	}

	return 0;
}

static bool isExistSymbolTable(char* name)
{
	for(int i = 0 ; i < VM_SYMTBLNUM ; ++i)
	{
		if(symbolTable[i].name != NULL)
		{
			if((strncmp(symbolTable[i].name, name, strlen(name)) == 0) && (strlen(symbolTable[i].name) == strlen(name)))
			{
				return true;
			}
		}
	}

	return false;
}

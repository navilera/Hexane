/*
 * Main.c
 *
 *  Created on: Aug 19, 2017
 *      Author: maanu
 */

#include "Main.h"
#include "Shell.h"
#include "Lexer.h"
#include "Parser.h"
#include "CodeGen.h"
#include "VirtualMachine.h"

static bool checkInvalidSymbol(Token_t* symlist, const char* line);

int main(int argc, char* argv[])
{
	printf("Hexane 1.0.0\n");

	while(true)
	{
		char* line = Shell_GetLine();

		Token_t* symlist = Lexer_GetTok(line);
		if(!checkInvalidSymbol(symlist, line))
		{
			continue;
		}

		ParserNode_t* parseTree = Parser_Parse(symlist);
		if(parseTree == NULL)
		{
			printf("Syntax error\n");
			continue;
		}

		CodegenList_t* codelist = CodeGen_Compile(parseTree);
		if(codelist == NULL)
		{
			printf("Compile error\n");
			continue;
		}

		VmStack_t* sp = Vm_Run(codelist);
		if(sp == NULL)
		{
			printf("VM error\n");
			printf("%s\n", Vm_GetErrorMsg());
			continue;
		}

		if(!Vm_IsStackEmpty(sp))
		{
			printf("%s\n", Vm_GetStackValue(sp));
		}
	}

	return 0;
}

static bool checkInvalidSymbol(Token_t* symlist, const char* line)
{
	for(Token_t *sym = symlist; *sym != TOK_NOSYM ; ++sym)
	{
		if(*sym == TOK_ERR)
		{
			printf("Invalid Token\n");
			printf("%s", line);
			uint32_t errpos = Lexer_GetErrorPos();
			for(int i = 0; i < errpos ; ++i)
			{
				putchar(' ');
			}
			putchar('^');putchar('\n');
			return false;
		}
	}

	return true;
}

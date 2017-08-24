/*
 * Main.c
 *
 *  Created on: Aug 19, 2017
 *      Author: maanu
 */

#include "Main.h"
#include "Shell.h"
#include "Lexer.h"

int main(int argc, char* argv[])
{
	printf("Hexane 1.0.0\n");

	while(true)
	{
		char* line = Shell_GetLine();

		printf("--> %s", line);

		uint32_t symindex = 0;
		Symbol_t* symlist = Lexer_GetSym(line);
		for(Symbol_t *sym = symlist; *sym != SYM_NOSYM ; ++sym)
		{
			printf("sym:%d ", *sym);
			if(*sym == SYM_INT)
			{
				printf("int:%"PRIx64"\n", Lexer_GetIntVal(symindex));
			}
			if(*sym == SYM_ID)
			{
				printf("id:%s\n", Lexer_GetIdName(symindex));
			}
			if(*sym == SYM_ERR)
			{
				printf("Syntax error!!\n");
				printf("%s", line);
				uint32_t errpos = Lexer_GetErrorPos();
				for(int i = 0; i < errpos ; ++i)
				{
					putchar(' ');
				}
				putchar('^');putchar('\n');
			}
			symindex++;
		}
	}

	return 0;
}

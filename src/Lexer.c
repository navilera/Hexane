/*
 * Lexer.c
 *
 *  Created on: Aug 21, 2017
 *      Author: maanu
 */

#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include "Lexer.h"

/*
 * SYM_INT	[0-9a-fA-F]+	// default integer is hex value
 * ID		$[a-zA-Z][0-9a-zA-z_]*
 */

static Symbol_t symbolBuffer[LEX_SYM_NUM_LIMIT];
static uint64_t intValBuffer[LEX_SYM_NUM_LIMIT];
static char*	idNameBuffer[LEX_SYM_NUM_LIMIT];

Symbol_t* Lexer_GetSym(char* line)
{
	int index = 0;
	char* ch = '\0';

	for(int i = 0; i < LEX_SYM_NUM_LIMIT; ++i)
	{
		char* idNamePtr = idNameBuffer[i];
		if(idNamePtr != NULL)
		{
			free(idNamePtr);
		}
	}

	memset(symbolBuffer, 0, sizeof(symbolBuffer));
	memset(intValBuffer, 0, sizeof(intValBuffer));
	memset(idNameBuffer, 0, sizeof(idNameBuffer));

	for(ch = (char*)line; ch != '\0'; ++ch)
	{
		switch(*ch)
		{
		case ' ':
			continue;
		case '\n':
			return symbolBuffer;
		}

		if((*ch >= '0' && *ch <= '9') ||
		   (*ch >= 'a' && *ch <= 'f') ||
		   (*ch >= 'A' && *ch <= 'F'))
		{
			intValBuffer[index] = 0;
			while((*ch >= '0' && *ch <= '9') ||
				  (*ch >= 'a' && *ch <= 'f') ||
				  (*ch >= 'A' && *ch <= 'F'))
			{
				while(*ch >= '0' && *ch <= '9')
				{
					intValBuffer[index] = (intValBuffer[index] * 16) + (ch - '0');
					++ch;
				}
				while(*ch >= 'a' && *ch <= 'f')
				{
					intValBuffer[index] = (intValBuffer[index] * 16) + (ch - 'a' + 10);
					++ch;
				}
				while(*ch >= 'A' && *ch <= 'F')
				{
					intValBuffer[index] = (intValBuffer[index] * 16) + (ch - 'A' + 10);
					++ch;
				}
			}
			symbolBuffer[index] = SYM_INT;
			++index;
		}
	}

	return symbolBuffer;
}

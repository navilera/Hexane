/*
 * Lexer.c
 *
 *  Created on: Aug 21, 2017
 *      Author: maanu
 */

#include "Main.h"
#include "Lexer.h"

/*
 * SYM_INT	[0-9a-fA-F]+ | [uU][0-9]	// default integer is hex value
 * SYM_ID	$[0-9a-zA-z_]+
 * SYM_STR	".*"
 * SYM_COL 	:
 * SYM_DOT	.
 * SYM_LPAR (
 * SYM_RPAR )
 * SYM_PLUS +
 * SYM_MIN	-
 * SYM_MUL	*
 * SYM_DIV	/
 * SYM_RIM	%
 * SYM_EQU	=
 */

static Symbol_t symbolBuffer[LEX_SYM_NUM_LIMIT];
static uint64_t intValBuffer[LEX_SYM_NUM_LIMIT];
static char*	idNameBuffer[LEX_SYM_NUM_LIMIT];
static uint32_t symErrorPosition;

static char		idSymBuffer[LEX_ID_SYM_LEN];

static void initAllSymbolBuffer(void);

static bool getHexInt(char** chBack, uint64_t* intValBack);
static bool getDecInt(char** chBack, uint64_t* intValBack);
static bool getId(char** chBack, char** idSymStr);
static bool getStr(char** chBack, char** idSymStr);
static bool cancelPrevSymbol(int index, char* ch, char* line);
static void setError(int index, char* ch, char* line);

#define AddSimpleSym(sym) {symbolBuffer[index] = sym; ++index; ++ch;}

Symbol_t* Lexer_GetSym(char* line)
{
	int index = 0;
	char* ch = '\0';

	uint64_t intVal = 0;
	char* idSymStr = NULL;

	initAllSymbolBuffer();

	ch = line;
	while(*ch != '\0')
	{
		if(*ch == ' ')
		{
			++ch;
			continue;
		}
		else if(*ch == ':') {AddSimpleSym(SYM_COL);}
		else if(*ch == '.') {AddSimpleSym(SYM_DOT);}
		else if(*ch == '(') {AddSimpleSym(SYM_LPAR);}
		else if(*ch == ')') {AddSimpleSym(SYM_RPAR);}
		else if(*ch == '+') {AddSimpleSym(SYM_PLUS);}
		else if(*ch == '-') {AddSimpleSym(SYM_MIN);}
		else if(*ch == '*') {AddSimpleSym(SYM_MUL);}
		else if(*ch == '/') {AddSimpleSym(SYM_DIV);}
		else if(*ch == '%') {AddSimpleSym(SYM_RIM);}
		else if(*ch == '=') {AddSimpleSym(SYM_EQU);}
		else if(*ch == '\n')
		{
			return symbolBuffer;
		}
		else if((*ch >= '0' && *ch <= '9') || (*ch >= 'a' && *ch <= 'f') || (*ch >= 'A' && *ch <= 'F'))
		{
			// cancel previous symbol if invalid token happens
			if(cancelPrevSymbol(index, ch, line))
			{
				return symbolBuffer;
			}

			if(getHexInt(&ch, &intVal))
			{
				intValBuffer[index] = intVal;
				symbolBuffer[index] = SYM_INT;
				++index;
			}
		}
		else if(*ch == '$')
		{
			// cancel previous symbol if invalid token happens
			if(cancelPrevSymbol(index, ch, line))
			{
				return symbolBuffer;
			}

			if(getId(&ch, &idSymStr))
			{
				idNameBuffer[index] = idSymStr;
				symbolBuffer[index] = SYM_ID;
				++index;
			}
			else
			{
				setError(index, ch, line);
				return symbolBuffer;
			}
		}
		else if(*ch == '"')
		{
			// cancel previous symbol if invalid token happens
			if(cancelPrevSymbol(index, ch, line))
			{
				return symbolBuffer;
			}
			if(getStr(&ch, &idSymStr))
			{
				idNameBuffer[index] = idSymStr;
				symbolBuffer[index] = SYM_STR;
				++index;
			}
			else
			{
				setError(index, ch, line);
				return symbolBuffer;
			}
		}
		else if(*ch == 'u' || *ch == 'U')
		{
			// cancel previous symbol if invalid token happens
			if(cancelPrevSymbol(index, ch, line))
			{
				return symbolBuffer;
			}

			if(getDecInt(&ch, &intVal))
			{
				intValBuffer[index] = intVal;
				symbolBuffer[index] = SYM_INT;
				++index;
			}
			else
			{
				setError(index, ch, line);
				return symbolBuffer;
			}
		}
		else
		{
			// cancel previous symbol if invalid token happens
			if(cancelPrevSymbol(index, ch, line))
			{
				return symbolBuffer;
			}
			setError(index, ch, line);
			return symbolBuffer;
		}
	}

	return symbolBuffer;
}

uint64_t Lexer_GetIntVal(uint32_t symIndex)
{
	return intValBuffer[symIndex];
}

char* Lexer_GetIdName(uint32_t symIndex)
{
	return idNameBuffer[symIndex];
}

uint32_t Lexer_GetErrorPos(void)
{
	return symErrorPosition;
}

static void initAllSymbolBuffer(void)
{
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

	symErrorPosition = 0;
}

static bool getHexInt(char** chBack, uint64_t* intValBack)
{
	uint64_t intVal = 0;
	char* ch = *chBack;

	*intValBack = 0;

	if((*ch >= '0' && *ch <= '9') || (*ch >= 'a' && *ch <= 'f') || (*ch >= 'A' && *ch <= 'F'))
	{
		while((*ch >= '0' && *ch <= '9') || (*ch >= 'a' && *ch <= 'f') || (*ch >= 'A' && *ch <= 'F'))
		{
			while(*ch >= '0' && *ch <= '9')
			{
				intVal = (uint64_t)((intVal * 16) + (*ch - '0'));
				++ch;
			}
			while(*ch >= 'a' && *ch <= 'f')
			{
				intVal = (uint64_t)((intVal * 16) + (*ch - 'a' + 10));
				++ch;
			}
			while(*ch >= 'A' && *ch <= 'F')
			{
				intVal = (uint64_t)((intVal * 16) + (*ch - 'A' + 10));
				++ch;
			}
		}
		*intValBack = intVal;
		*chBack = ch;
		return true;
	}
	return false;
}

static bool getDecInt(char** chBack, uint64_t* intValBack)
{
	uint64_t intVal = 0;
	char* ch = *chBack;

	*intValBack = 0;

	++ch;

	if((*ch >= '0' && *ch <= '9'))
	{
		while((*ch >= '0' && *ch <= '9'))
		{
			intVal = (uint64_t)((intVal * 10) + (*ch - '0'));
			++ch;
		}
		*intValBack = intVal;
		*chBack = ch;
		return true;
	}
	return false;
}

static bool getId(char** chBack, char** idSymStr)
{
	char* ch = *chBack;

	int i = 0;

	memset(idSymBuffer, 0, sizeof(idSymBuffer));
	++ch;

	if((*ch >= '0' && *ch <= '9') || (*ch >= 'a' && *ch <= 'z') || (*ch >= 'A' && *ch <= 'Z') || (*ch == '_'))
	{
		while((*ch >= '0' && *ch <= '9') || (*ch >= 'a' && *ch <= 'z') || (*ch >= 'A' && *ch <= 'Z') || (*ch == '_'))
		{
			idSymBuffer[i++] = *ch;
			++ch;
		}
		idSymBuffer[i] = '\0';

		*idSymStr = (char*)malloc(strlen(idSymBuffer) + 1);
		memset(*idSymStr, 0, strlen(idSymBuffer) + 1);
		memcpy(*idSymStr, idSymBuffer, strlen(idSymBuffer));

		*chBack = ch;

		return true;
	}

	return false;
}

static bool getStr(char** chBack, char** idSymStr)
{
	char* ch = *chBack;

	int i = 0;

	memset(idSymBuffer, 0, sizeof(idSymBuffer));
	++ch;

	if(*ch != '"')
	{
		while(*ch != '"')
		{
			idSymBuffer[i++] = *ch;
			++ch;
		}
		idSymBuffer[i] = '\0';

		*idSymStr = (char*)malloc(strlen(idSymBuffer) + 1);
		memset(*idSymStr, 0, strlen(idSymBuffer) + 1);
		memcpy(*idSymStr, idSymBuffer, strlen(idSymBuffer));

		++ch;
		*chBack = ch;

		return true;
	}

	return false;
}

static bool cancelPrevSymbol(int index, char* ch, char* line)
{
	if(symbolBuffer[index-1] == SYM_INT ||
	symbolBuffer[index-1] == SYM_ID)
	{
		// cancel in this case
		// 390acd0adskw
		//          ^
		if(*(ch-1) != ' ')
		{
			// if in this case, do not cancel
			// 23824 keowd
			//       ^
			setError((index-1), ch, line);
			symErrorPosition = (uint32_t)(ch - line);
			return true;
		}
	}

	return false;
}

static void setError(int index, char* ch, char* line)
{
	symbolBuffer[index] = SYM_ERR;
	symErrorPosition = (uint32_t)(ch - line);
}

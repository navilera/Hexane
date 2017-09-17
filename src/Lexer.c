/*
 * Lexer.c
 *
 *  Created on: Aug 21, 2017
 *      Author: maanu
 */

#include "Main.h"
#include "Lexer.h"

/*
 * TOK_INT	[0-9a-fA-F]+ | [uU][0-9]	// default integer is hex value
 * TOK_ID	$[0-9a-zA-z_]+
 * TOK_STR	".*"
 * TOK_COL 	:
 * TOK_DOT	.
 * TOK_LPAR (
 * TOK_RPAR )
 * TOK_PLUS +
 * TOK_MIN	-
 * TOK_MUL	*
 * TOK_DIV	/
 * TOK_RIM	%
 * TOK_EQU	=
 */

static Token_t tokenBuffer[LEX_TOK_NUM_LIMIT];
static uint64_t intValBuffer[LEX_TOK_NUM_LIMIT];
static char*	idNameBuffer[LEX_TOK_NUM_LIMIT];
static uint32_t symErrorPosition;

static char		idTokBuffer[LEX_ID_TOK_LEN];

static void initAllTokenBuffer(void);

static bool getHexInt(char** chBack, uint64_t* intValBack);
static bool getDecInt(char** chBack, uint64_t* intValBack);
static bool getId(char** chBack, char** idTokStr);
static bool getStr(char** chBack, char** idTokStr);
static bool cancelPrevToken(int index, char* ch, char* line);
static void setError(int index, char* ch, char* line);

#define AddSimpleTok(tok) {tokenBuffer[index] = tok; ++index; ++ch;}

Token_t* Lexer_GetTok(char* line)
{
	int index = 0;
	char* ch = '\0';

	uint64_t intVal = 0;
	char* idTokStr = NULL;

	initAllTokenBuffer();

	ch = line;
	while(*ch != '\0')
	{
		if(*ch == ' ')
		{
			++ch;
			continue;
		}
		else if(*ch == ':') {AddSimpleTok(TOK_COL);}
		else if(*ch == '.') {AddSimpleTok(TOK_DOT);}
		else if(*ch == '(') {AddSimpleTok(TOK_LPAR);}
		else if(*ch == ')') {AddSimpleTok(TOK_RPAR);}
		else if(*ch == '+') {AddSimpleTok(TOK_PLUS);}
		else if(*ch == '-') {AddSimpleTok(TOK_MIN);}
		else if(*ch == '*') {AddSimpleTok(TOK_MUL);}
		else if(*ch == '/') {AddSimpleTok(TOK_DIV);}
		else if(*ch == '%') {AddSimpleTok(TOK_RIM);}
		else if(*ch == '=') {AddSimpleTok(TOK_EQU);}
		else if(*ch == '\n')
		{
			return tokenBuffer;
		}
		else if((*ch >= '0' && *ch <= '9') || (*ch >= 'a' && *ch <= 'f') || (*ch >= 'A' && *ch <= 'F'))
		{
			// cancel previous symbol if invalid token happens
			if(cancelPrevToken(index, ch, line))
			{
				return tokenBuffer;
			}

			if(getHexInt(&ch, &intVal))
			{
				intValBuffer[index] = intVal;
				tokenBuffer[index] = TOK_INT;
				++index;
			}
		}
		else if(*ch == '$')
		{
			// cancel previous symbol if invalid token happens
			if(cancelPrevToken(index, ch, line))
			{
				return tokenBuffer;
			}

			if(getId(&ch, &idTokStr))
			{
				idNameBuffer[index] = idTokStr;
				tokenBuffer[index] = TOK_ID;
				++index;
			}
			else
			{
				setError(index, ch, line);
				return tokenBuffer;
			}
		}
		else if(*ch == '"')
		{
			// cancel previous symbol if invalid token happens
			if(cancelPrevToken(index, ch, line))
			{
				return tokenBuffer;
			}
			if(getStr(&ch, &idTokStr))
			{
				idNameBuffer[index] = idTokStr;
				tokenBuffer[index] = TOK_STR;
				++index;
			}
			else
			{
				setError(index, ch, line);
				return tokenBuffer;
			}
		}
		else if(*ch == 'u' || *ch == 'U')
		{
			// cancel previous symbol if invalid token happens
			if(cancelPrevToken(index, ch, line))
			{
				return tokenBuffer;
			}

			if(getDecInt(&ch, &intVal))
			{
				intValBuffer[index] = intVal;
				tokenBuffer[index] = TOK_INT;
				++index;
			}
			else
			{
				setError(index, ch, line);
				return tokenBuffer;
			}
		}
		else
		{
			// cancel previous symbol if invalid token happens
			if(cancelPrevToken(index, ch, line))
			{
				return tokenBuffer;
			}
			setError(index, ch, line);
			return tokenBuffer;
		}
	}

	return tokenBuffer;
}

uint64_t Lexer_GetIntVal(uint32_t tokIndex)
{
	return intValBuffer[tokIndex];
}

char* Lexer_GetIdName(uint32_t tokIndex)
{
	return idNameBuffer[tokIndex];
}

uint32_t Lexer_GetErrorPos(void)
{
	return symErrorPosition;
}

static void initAllTokenBuffer(void)
{
	for(int i = 0; i < LEX_TOK_NUM_LIMIT; ++i)
	{
		char* idNamePtr = idNameBuffer[i];
		if(idNamePtr != NULL)
		{
			free(idNamePtr);
		}
	}

	memset(tokenBuffer, 0, sizeof(tokenBuffer));
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

static bool getId(char** chBack, char** idTokStr)
{
	char* ch = *chBack;

	int i = 0;

	memset(idTokBuffer, 0, sizeof(idTokBuffer));
	++ch;

	if((*ch >= '0' && *ch <= '9') || (*ch >= 'a' && *ch <= 'z') || (*ch >= 'A' && *ch <= 'Z') || (*ch == '_'))
	{
		while((*ch >= '0' && *ch <= '9') || (*ch >= 'a' && *ch <= 'z') || (*ch >= 'A' && *ch <= 'Z') || (*ch == '_'))
		{
			idTokBuffer[i++] = *ch;
			++ch;
		}
		idTokBuffer[i] = '\0';

		*idTokStr = (char*)malloc(strlen(idTokBuffer) + 1);
		memset(*idTokStr, 0, strlen(idTokBuffer) + 1);
		memcpy(*idTokStr, idTokBuffer, strlen(idTokBuffer));

		*chBack = ch;

		return true;
	}

	return false;
}

static bool getStr(char** chBack, char** idTokStr)
{
	char* ch = *chBack;

	int i = 0;

	memset(idTokBuffer, 0, sizeof(idTokBuffer));
	++ch;

	if(*ch != '"')
	{
		while(*ch != '"')
		{
			idTokBuffer[i++] = *ch;
			++ch;
		}
		idTokBuffer[i] = '\0';

		*idTokStr = (char*)malloc(strlen(idTokBuffer) + 1);
		memset(*idTokStr, 0, strlen(idTokBuffer) + 1);
		memcpy(*idTokStr, idTokBuffer, strlen(idTokBuffer));

		++ch;
		*chBack = ch;

		return true;
	}

	return false;
}

static bool cancelPrevToken(int index, char* ch, char* line)
{
	if(tokenBuffer[index-1] == TOK_INT ||
	tokenBuffer[index-1] == TOK_ID)
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
	tokenBuffer[index] = TOK_ERR;
	symErrorPosition = (uint32_t)(ch - line);
}

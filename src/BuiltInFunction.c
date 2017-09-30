/*
 * BuiltInFunction.c
 *
 *  Created on: Sep 24, 2017
 *      Author: maanu
 */

#include "Main.h"

#include "BuiltInFunction.h"

char strRetValue[BUILTINFUNC_RET_LEN_LIMIT];

static BuiltInFuncReg_t builtInFunctions[] = {
		{"dec", 		BLTINFUN_dec},
		{"bin",			BLTINFUN_bin},
		{NULL, 			BLTINFUN_No}
};

char* BuiltInFunc_Dec(uint64_t val)
{
	memset(strRetValue, 0, BUILTINFUNC_RET_LEN_LIMIT);

	sprintf(strRetValue, "u%lu", val);

	return strRetValue;
}

char* BuiltInFunc_Bin(uint64_t val)
{
	int maxBitLen = sizeof(uint64_t) * 8;
	memset(strRetValue, 0, BUILTINFUNC_RET_LEN_LIMIT);

	// find the first non-zero bit
	int firstNonZeroBitOffset = 0;
	for(int i = 1 ; i <= maxBitLen ; i++)
	{
		firstNonZeroBitOffset = (maxBitLen - i);
		int bitValue = (val >> firstNonZeroBitOffset) & 0x1;

		if(bitValue != 0)
		{
			break;
		}
	}

	// increase because start offset is 0 but calculating length need to start 1
	firstNonZeroBitOffset += 1;

	// calculate 4bit align offset
	int printLen = 4 * ((((firstNonZeroBitOffset - 1) + 4)) / 4);

	printf("fnonze:%d\n", firstNonZeroBitOffset);
	printf("printlen : %d\n", printLen);

	// make string return value
	char* ch = strRetValue;

	// print bit index
	for(int i = 1 ; i <= printLen ; i++)
	{
		sprintf(ch, "%d ", (printLen - i));
		ch += strlen(ch);
	}
	*ch++ = '\n';

	// print under line
	for(int i = 1 ; i <= printLen ; i++)
	{
		*ch++ = '=';
		if((printLen - i) >= 10)
		{
			*ch++ = '=';
		}
		if(((printLen - i)%4) == 0)
		{
			*ch++ = '|';
		}
		else
		{
			*ch++ = ' ';
		}
	}
	*ch++ = '\n';

	// print bit value
	for(int i = 1 ; i <= printLen ; i++)
	{
		int bitValue = (val >> (printLen - i)) & 0x1;
		if((printLen - i) >= 10)
		{
			*ch++ = ' ';
		}
		*ch++ = bitValue + '0';
		*ch++ = ' ';
	}
	*ch++ = '\n';

	return strRetValue;
}

BuiltInFuncId_t BuiltInFunc_Check(char* funcName)
{
	for(int i = 0 ; builtInFunctions[i].id != BLTINFUN_No ; ++i)
	{
		if(STR_CMP(builtInFunctions[i].name, funcName))
		{
			return builtInFunctions[i].id;
		}
	}
	return BLTINFUN_No;
}



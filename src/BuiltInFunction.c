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
		{NULL, 			BLTINFUN_No}
};

char* BuiltInFunc_Dec(uint64_t val)
{
	memset(strRetValue, 0, BUILTINFUNC_RET_LEN_LIMIT);

	sprintf(strRetValue, "u%lu", val);

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

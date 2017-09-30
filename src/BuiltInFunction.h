/*
 * BuiltInFunction.h
 *
 *  Created on: Sep 24, 2017
 *      Author: maanu
 */

#ifndef SRC_BUILTINFUNCTION_H_
#define SRC_BUILTINFUNCTION_H_

#define BUILTINFUNC_RET_LEN_LIMIT 1024

typedef enum {
	BLTINFUN_No = 0,
	BLTINFUN_dec,
	BLTINFUN_bin,
	NumberOfBuiltInFunctions
} BuiltInFuncId_t;

typedef struct _builtinfunction_registration_t_
{
	char* name;
	BuiltInFuncId_t id;
} BuiltInFuncReg_t;


BuiltInFuncId_t BuiltInFunc_Check(char* funcName);

char* BuiltInFunc_Dec(uint64_t val);
char* BuiltInFunc_Bin(uint64_t val);

#endif /* SRC_BUILTINFUNCTION_H_ */

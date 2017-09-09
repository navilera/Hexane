/*
 * CodeGen.h
 *
 *  Created on: Aug 29, 2017
 *      Author: maanu
 */

#ifndef SRC_CODEGEN_H_
#define SRC_CODEGEN_H_

typedef enum {
	Code_No = 0,
	Code_Str,
	Code_Ldr,
	Code_Push,
	Code_Pop,
	Code_Add,
	Code_Sub,
	Code_Mul,
	Code_Div,
	Code_Mod,
	Code_Halt,
	NumberOfCodes
}ItmdCode_t;

typedef enum {
	CodeType_Instr,
	CodeType_Addr,
	CodeType_Int,
	CodeType_Str,
	NumberOfCodeType
}CodeType_t;

typedef struct _codegen_codelist_t_
{
	uint64_t val;
	CodeType_t type;
} CodegenList_t;

#define CODE_NUM_LIMIT		1024

CodegenList_t* CodeGen_Compile(ParserNode_t* parseTree);

#endif /* SRC_CODEGEN_H_ */

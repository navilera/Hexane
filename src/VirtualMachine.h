/*
 * VirtualMachine.h
 *
 *  Created on: Sep 1, 2017
 *      Author: maanu
 */

#ifndef SRC_VIRTUALMACHINE_H_
#define SRC_VIRTUALMACHINE_H_

typedef struct _vm_symbol_table_t_
{
	char* 		name;
	uint64_t 	val;
} VmSymbolTable_t;

#define VM_SYMTBLNUM		1024
#define VM_STACKSIZE		2048
#define VM_ERRSTRLEN		1024
#define VM_RETSTRLEN		1024

uint64_t* Vm_Run(CodegenList_t* code);
bool Vm_IsStackEmpty(uint64_t* sp);
char* Vm_GetStackValue(uint64_t* sp);
char* Vm_GetErrorMsg(void);

#define VM_ERR_critical_error			"Critical Error!!!"
#define VM_ERR_not_enough_symbol_table	"Not enough symbol table entry number"
#define VM_ERR_Invalid_variable			"Invalid variable"

#endif /* SRC_VIRTUALMACHINE_H_ */

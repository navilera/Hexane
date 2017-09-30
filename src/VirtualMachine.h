/*
 * VirtualMachine.h
 *
 *  Created on: Sep 1, 2017
 *      Author: maanu
 */

#ifndef SRC_VIRTUALMACHINE_H_
#define SRC_VIRTUALMACHINE_H_

typedef enum {
	VmStackType_Addr,
	VmStackType_Int,
	VmStackType_Str,
	VmStackType_Str_NoQuote,
	NumberOfVmStackType
} VmStackType_t;

typedef struct _vm_stack_t_
{
	uint64_t val;
	VmStackType_t type;
} VmStack_t;

typedef struct _vm_symbol_table_t_
{
	char* 		name;
	VmStack_t 	val;
} VmSymbolTable_t;

#define VM_SYMTBLNUM		1024
#define VM_STACKSIZE		2048
#define VM_ERRSTRLEN		1024
#define VM_RETSTRLEN		1024

VmStack_t* Vm_Run(CodegenList_t* code);
bool Vm_IsStackEmpty(VmStack_t* sp);
char* Vm_GetStackValue(VmStack_t* sp);
char* Vm_GetErrorMsg(void);

#define VM_ERR_critical_error			"Critical Error!!!"
#define VM_ERR_not_enough_symbol_table	"Not enough symbol table entry number"
#define VM_ERR_Invalid_variable			"Invalid variable"
#define VM_ERR_undefined_function		"Undefined function"

#endif /* SRC_VIRTUALMACHINE_H_ */

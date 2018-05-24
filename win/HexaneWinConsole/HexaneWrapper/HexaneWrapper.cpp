// This is the main DLL file.

#include "stdafx.h"

#include "HexaneWrapper.h"

#include <vcclr.h> // for PtrToStringChars
#include <stdio.h> // for wprintf
#include <stdint.h>

#include "Main.h"
#include "Lexer.h"
#include "Parser.h"
#include "CodeGen.h"
#include "VirtualMachine.h"

namespace HexaneWrapper {

	String^ HexaneEngine::Run(String^ inputLine)
	{
		String^ ret;
		char* engineResult;

		//pin_ptr<const wchar_t> wchStr = PtrToStringChars(line);
		using namespace Runtime::InteropServices;
		char* line = (char*)(Marshal::StringToHGlobalAnsi(inputLine)).ToPointer();

		//wprintf(wchStr);

		Token_t* toklist = Lexer_GetTok(line);
		/*
		if (!checkInvalidSymbol(symlist, line))
		{
			continue;
		}
		*/

		ParserNode_t* parseTree = Parser_Parse(toklist);
		if (parseTree == NULL)
		{
			engineResult = "Syntax error";
			goto OUT;
		}

		CodegenList_t* codelist = CodeGen_Compile(parseTree);
		if (codelist == NULL)
		{
			engineResult = "Compile error";
			goto OUT;
		}

		VmStack_t* sp = Vm_Run(codelist);
		if (sp == NULL)
		{
			engineResult = (char*)malloc(256);
			memset(engineResult, 0, 256);

			char *errMsg = "VM error\n";
			strncat(engineResult, errMsg, strlen(errMsg));
			strncat(engineResult, Vm_GetErrorMsg(), strlen(Vm_GetErrorMsg()));
			goto OUT;
		}

		if (!Vm_IsStackEmpty(sp))
		{
			engineResult = Vm_GetStackValue(sp);
		}

	OUT:
		ret = gcnew String(engineResult);
		return ret;
	}
}
// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#define _CPU_ENABLE_EXEPTIONS 1

#define DEBUG

#include "Compiler.hpp"

INT main()
{
	Debugger::SetColorConsole(Debugger::TextColors::White);

	Compiler<> comp;
	comp.toComTextFile("Test");
	
	system("pause");
    return 0;
}
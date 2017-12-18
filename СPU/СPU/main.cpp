// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#define _CPU_ENABLE_EXEPTIONS 1

#define DEBUG

#include "Compiler.hpp"

INT main(INT argc, CHAR *argv[])
{
	std::string file = (argc >= 2 ? argv[1] : "Test");

	NDebugger::SetColorConsole(NDebugger::TextColors::White);

	Compiler<> comp;
	comp.fromTextFile(file);

	system("pause");
    return 0;
}
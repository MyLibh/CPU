// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include <iostream>

#define CPU_GUARD_LVL 2

#include "Compiler.hpp"

using namespace NCompiler;

int main(int argc, char *argv[])
{
	std::ios::sync_with_stdio(false);
	
	std::string file((argc >= 2 ? argv[1] : "..\\..\\src\\Tests\\Text\\Text1"));

	try
	{
		NStack::Stack<> a;
	
		NDebugger::SetColorConsole(NDebugger::TextColor::White);

 		Compiler<> comp;
		comp.fromBinComFile(file);
	}
	catch (const std::exception &exc)
	{
		std::cout << exc.what();
	}
	catch (...)
	{
		std::cout << "Unhandled exeption\n";
	}

	system("pause");
    return 0;
}
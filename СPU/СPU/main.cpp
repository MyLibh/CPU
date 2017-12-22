// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include <iostream>

#include "Compiler.hpp"

using namespace NCompiler;

INT main(INT argc, CHAR *argv[])
{
	std::string file = (argc >= 2 ? argv[1] : "Text1");

	try
	{
		NDebugger::SetColorConsole(NDebugger::TextColors::White);

		Compiler<> comp;
		comp.fromComFile("Tests\\Text\\" + file);
	}
	catch (CONST std::exception &exc)
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
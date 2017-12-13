// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "Compiler.hpp"

INT main()
{
	Compiler<> comp;
	comp.FromComTextFile("TestComText");

    system("pause");
    return 0;
}
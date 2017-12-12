// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "CPU.hpp"
#include "Parser.hpp"

INT main()
{
    /*CPU<> proc1, 
		  proc2;

    proc1.compileFromFile("Test");
	
	proc2.push(28);
    proc2.push(28);
    proc2.dumpStack();
    proc2.push(17);
    proc2.dumpStack();
    proc2.mul();
    proc2.dumpStack();

	proc2 = proc1; 
	proc1.dumpStack();
	proc2.dumpStack();

	CPU<> proc3(std::move(proc2));
	proc3.dumpStack();
	proc2.dumpStack();

	proc3.sqrt();
	*/

	//Compiler<> comp;
	//comp.toTextFile("Test");

	delete &ParseCode("push15");

    system("pause");
    return 0;
}
// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

/* cmd -D_SCL_SECURE_NO_WARNINGS */

#define CPU_GUARD_LVL 2

#include "Compiler.hpp"

using namespace NCompiler;

int main(int argc, char *argv[])
{
	std::ios::sync_with_stdio(false);
	
	NDebugger::SetColorConsole(NDebugger::TextColor::White);
	Logger::init();

	try
	{
		std::string file((argc >= 2 ? argv[1] : "..\\..\\src\\Tests\\Text\\Text1"));
		
		// |   GADJET    |  RELEASE  |   DEBUG   |
		// |=-=-=-=-=-=-=|=-=-=-=-=-=|=-=-=-=-=-=|
		// | PC-MAIN     | 7sec      | 9sec      |
		// | PC-TWINK    | 11sec     |           |
		// | LAPTOP-MAIN | 2min40sec | 2min46sec |

		for (int i = 0; i < 100; ++i) 
		{
			Compiler<> comp;
			comp.fromTextFile(file);
		}
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
	Logger::close();
    
	return 0;
}
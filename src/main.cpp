// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

/* cmd -D_SCL_SECURE_NO_WARNINGS _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING */

#define GUARD_LVL 3

#include "Compiler.hpp"

using namespace NCompiler;

// |=-=-=-=-=-=-=|=-=-=-=-=-=|=-=-=-=-=-=|
// |   GADJET    |  RELEASE  |   DEBUG   |
// |=-=-=-=-=-=-=|=-=-=-=-=-=|=-=-=-=-=-=|
// | PC-MAIN     | 7sec      | 9sec      |
// | PC-TWINK    | 11sec     |           |
// | LAPTOP-MAIN | 2min40sec | 2min46sec |
// |=-=-=-=-=-=-=|=-=-=-=-=-=|=-=-=-=-=-=|

int main(int argc, char *argv[])
{
	std::ios::sync_with_stdio(false);
	
	NDebugger::SetConsoleColor(NDebugger::Colors::White);
	Logger::init();

	try
	{
		std::string file((argc >= 2 ? argv[1] : "..\\..\\src\\Tests\\Text\\Text1Com"));		

		Compiler<> comp;
		comp.fromComFile(file);
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
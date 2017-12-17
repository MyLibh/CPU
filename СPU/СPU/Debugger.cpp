#include <Windows.h>

#include "Debugger.hpp"

namespace Debugger
{
	WORD SetColorConsole(TextColors color, TextColors background /* = TextColors::Black */)
	{
		static TextColors oldText = TextColors::White,
			              oldBckg = TextColors::Black;

		std::swap(oldText, color);
		std::swap(oldBckg, background);

		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), oldText);

		return ((background << 4) | color);
	}

	inline WORD SetColorConsole(WORD color)
	{
		return SetColorConsole(static_cast<TextColors>(color ^ ((color >> 4) << 4)), static_cast<TextColors>(color >> 4)); // QUEST: 0x03 does not work
	}

	VOID Error(CRSTRING error) 
	{
		auto old = SetColorConsole(TextColors::Red);

		std::cerr << "[ERROR] " << error << std::endl;

		SetColorConsole(old);
	}

	VOID Info(CRSTRING info, TextColors color /* = TextColors::White*/, bool endline /* = TRUE */) 
	{
		auto old = SetColorConsole(color);

		std::cout << info;

		if(endline) std::cout << std::endl;

		SetColorConsole(old);
	}
};

#undef _WINDOWS_
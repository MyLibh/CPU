// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include <Windows.h>

#include "Debugger.hpp"

namespace NDebugger
{
	WORD SetColorConsole(TextColors color, TextColors background /* = TextColors::Black */)
	{
		static TextColors oldText = TextColors::White,
			              oldBckg = TextColors::Black;

		std::swap(oldText, color);
		std::swap(oldBckg, background);

		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), ((oldBckg << 4) | oldText));

		return ((background << 4) | color);
	}

	inline WORD SetColorConsole(WORD color)
	{
		return SetColorConsole(static_cast<TextColors>(color & 0x0F), static_cast<TextColors>(color & 0xF0));
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
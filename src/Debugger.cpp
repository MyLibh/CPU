// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include <Windows.h> // SetConsoleTextAttribute, GetStdHandle
#include <iostream>  // std::cerr, std::cout

#include "Debugger.hpp"

namespace NDebugger
{
	WORD SetColorConsole(TextColors color, TextColors background /* = TextColors::Black */)
	{
		static TextColors oldText = TextColors::White,
			              oldBckg = TextColors::Black;

		std::swap(oldText, color);
		std::swap(oldBckg, background);

		static HANDLE hHandle = GetStdHandle(STD_OUTPUT_HANDLE);
		if(!SetConsoleTextAttribute(hHandle, ((oldBckg << 4) | oldText))) throw std::exception("[NDebugger::SetColorConsole] \"SetConsoleTextAttribute returned false\"\n");

		return ((background << 4) | color); // Return old text attribute
	}

	static WORD SetColorConsole(WORD color)
	{
		return SetColorConsole(static_cast<TextColors>(color & 0x0F), static_cast<TextColors>(color & 0xF0));
	}

	VOID Error(CRSTRING error) 
	{
		auto old = SetColorConsole(TextColors::Red);

		std::cerr << "[ERROR] " << error << std::endl;

		SetColorConsole(old);
	}

	VOID Info(CRSTRING info, TextColors color /* = TextColors::White */, bool endline /* = TRUE */) 
	{
		auto old = SetColorConsole(color);

		std::cout << info;

		if(endline) std::cout << std::endl;

		SetColorConsole(old);
	}

	VOID Debug(CRSTRING debugInfo, TextColors color /* = TextColors::White */, bool endline /* = TRUE */)
	{
		Info("[DEBUG] ", TextColors::Brown, FALSE);
		Info(debugInfo, color, endline);
	}
} // namespace NDebugger

#undef _WINDOWS_
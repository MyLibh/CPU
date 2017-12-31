// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include <Windows.h> // SetConsoleTextAttribute, GetStdHandle
#include <iostream>  // std::cerr, std::cout
#include <string>    // std::string

#include "Debugger.hpp"

namespace NDebugger
{
	WORD SetColorConsole(TextColor color, TextColor background /* = TextColor::Black */)
	{
		static TextColor oldText = TextColor::White,
			             oldBckg = TextColor::Black;
		
		std::swap(oldText, color);
		std::swap(oldBckg, background);

		static HANDLE hHandle = GetStdHandle(STD_OUTPUT_HANDLE);
		if(!SetConsoleTextAttribute(hHandle, ((oldBckg << 4) | oldText))) throw std::exception("[NDebugger::SetColorConsole] \"SetConsoleTextAttribute returned false\"\n");

		return ((background << 4) | color); // Return old text attribute
	}

	static WORD SetColorConsole(WORD color)
	{
		return SetColorConsole(static_cast<TextColor>(color & 0x0F), static_cast<TextColor>(color & 0xF0));
	}

	VOID Error(CRSTRING error) 
	{
		auto old = SetColorConsole(TextColor::Red);

		std::cerr << "[ERROR] " << error << std::endl;

		SetColorConsole(old);
	}

	VOID Info(CRSTRING info, TextColor color /* = TextColor::White */, bool endline /* = TRUE */, std::ostream &rOste /* = std::cout */) 
	{
		auto old = SetColorConsole(color);

		rOste << info;

		if(endline) rOste << std::endl;

		SetColorConsole(old);
	}

	VOID Debug(CRSTRING debugInfo, TextColor color /* = TextColor::White */, bool endline /* = TRUE */)
	{
		Info("[DEBUG] ", TextColor::Brown, FALSE);
		Info(debugInfo, color, endline);
	}
} // namespace NDebugger

#undef _WINDOWS_
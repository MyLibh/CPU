// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include <Windows.h> // SetConsoleTextAttribute, GetStdHandle
#include <string>    // std::string_view

#include "Debugger.hpp"

namespace NDebugger
{
	unsigned short SetColorConsole(TextColor color, TextColor background /* = TextColor::Black */)
	{
		static TextColor sOldText = TextColor::White,
						 sOldBckg = TextColor::Black;
		
		std::swap(sOldText, color); 
		std::swap(sOldBckg, background);

		static HANDLE hHandle = GetStdHandle(STD_OUTPUT_HANDLE); // Get the handle to change
		if(!SetConsoleTextAttribute(hHandle, ((static_cast<WORD>(sOldBckg) << 4) | static_cast<WORD>(sOldText)))) 
			throw std::runtime_error(std::string("[") + __FUNCTION__ + "SetConsoleTextAttribute returned false\n");

		return ((static_cast<WORD>(background) << 4) | static_cast<WORD>(color)); // Return old text attribute
	}

	static unsigned short SetColorConsole(unsigned short color)
	{
		return SetColorConsole(static_cast<TextColor>(color & 0x0F), static_cast<TextColor>(color & 0xF0));
	}

	void Error(std::string_view error, std::ostream &rOstr /* = std::cerr */) 
	{
		auto old = SetColorConsole(TextColor::Red);

		rOstr << "[ERROR] " << error.data() << std::endl;

		SetColorConsole(old);
	}

	void Info(std::string_view info, TextColor color /* = TextColor::White */, bool endline /* = TRUE */, std::ostream &rOstr /* = std::cout */)
	{
		auto old = SetColorConsole(color);

		rOstr << info.data();

		if(endline) 
			rOstr << std::endl;

		SetColorConsole(old);
	}

	void Debug(std::string_view debugInfo, TextColor color /* = TextColor::White */, bool endline /* = TRUE */, std::ostream &rOstr /* = std::cout */)
	{
		Info("[DEBUG] ", TextColor::Brown, false, rOstr);
		Info(debugInfo, color, endline, rOstr);
	}
} // namespace NDebugger

#undef _WINDOWS_
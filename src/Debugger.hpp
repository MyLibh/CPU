#pragma once

#include <iostream> // std::ostream, std::cerr, std::cout

namespace NDebugger
{
	enum class TextColor : unsigned short
	{
		Black        = 0x00,
		Blue         = 0x01,
		Green        = 0x02,
		Cyan         = 0x03,
		Red          = 0x04,
		Magenta      = 0x05,
		Brown        = 0x06,
		LightGray    = 0x07,
		DarkGray     = 0x08,
		LightBlue    = 0x09,
		LightGreen   = 0x0A,
		LightCyan    = 0x0B,
		LightRed     = 0x0C,
		LightMagenta = 0x0D,
		Yellow       = 0x0E,
		White        = 0x0F
	};
	
	unsigned short SetColorConsole(TextColor, TextColor = TextColor::Black);

	void Error(std::string_view, std::ostream& = std::cerr); 
	void Info(std::string_view, TextColor = TextColor::White, bool = true, std::ostream& = std::cout);
	void Debug(std::string_view, TextColor = TextColor::White, bool = true, std::ostream& = std::cout);
} //namespace NDebugger
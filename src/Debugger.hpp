#pragma once

#include <iostream>

#include "MyTypedefs.hpp"

namespace NDebugger
{
	enum TextColor : WORD
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
	
	WORD SetColorConsole(TextColor, TextColor = TextColor::Black); 

	VOID Error(CRSTRING); 
	VOID Info(CRSTRING, TextColor = TextColor::White, bool = TRUE, std::ostream& = std::cout);
	VOID Debug(CRSTRING, TextColor = TextColor::White, bool = TRUE);
} //namespace NDebugger
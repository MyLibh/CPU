 #pragma once

#include <array> // std::array

#include "Debugger.hpp"

class Logger;

namespace NParser
{
	CONST SIZE_T MAX_LINE_LENGTH = 1 << 8;

	struct Operation
	{
		static CONST WORD MAX_ARGS = (1 << 2) - 1;

		std::string                       cmd;
		std::array<std::string, MAX_ARGS> args;

		explicit Operation() _NOEXCEPT;
		Operation(CONST Operation&);
		Operation(Operation&&);
		~Operation();

		Operation &operator=(CONST Operation&);
		Operation &operator=(Operation&&);

		VOID dump(std::ostream& = std::cout) const;
	};

	//===============================================================================================================================================
	
	Logger &operator<<(Logger&, CONST Operation&);

	//===============================================================================================================================================

	Operation ParseCode(CONST CHAR*);

	BOOL Move2Label(std::ifstream&, CRSTRING, std::streampos = std::ios::beg);

	BOOL Move2LabelBin(std::ifstream&, CRSTRING, std::streampos = std::ios::beg);
	
} // namespace NParser
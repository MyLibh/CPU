 #pragma once

#include <array> // std::array

#include "MyTypedefs.hpp"
#include "Debugger.hpp"

namespace NParser
{
	CONST SIZE_T MAX_LINE_LENGTH = 1 << 8;

	struct Operation
	{
		static CONST WORD MAX_ARGS = 1 << 2;

		std::string                       cmd;
		std::array<std::string, MAX_ARGS> args;

		explicit Operation() noexcept;
		Operation(CONST Operation&);
		Operation(Operation&&);
		~Operation();

		Operation &operator=(CONST Operation&);
		Operation &operator=(Operation&&);

		VOID dump() const;
	};

	Operation *ParseCode(CONST CHAR*);

	std::ifstream FindLabel(std::ifstream&, CRSTRING);
} // namespace NParser
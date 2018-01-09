 #pragma once

#include <array> // std::array

#include "Debugger.hpp"

class Logger;

namespace NParser
{
	constexpr size_t MAX_LINE_LENGTH = 1 << 8;

	struct Operation
	{
		static constexpr unsigned short MAX_ARGS = (1 << 2) - 1;

		std::string                       cmd;
		std::array<std::string, MAX_ARGS> args;

		explicit Operation()        = default;
		Operation(const Operation&) = default;
		Operation(Operation&&)      = default;
		~Operation()                = default;

		Operation &operator=(const Operation&) noexcept;
		Operation &operator=(Operation&&)      noexcept;

		void dump(std::ostream& = std::cout) const;
	};

	//===============================================================================================================================================
	
	Logger &operator<<(Logger&, const Operation&);

	//===============================================================================================================================================

	Operation ParseCode(std::string_view);

	bool Move2Label(std::ifstream&, std::string_view, std::streampos = std::ios::beg);

	bool Move2LabelBin(std::ifstream&, std::string_view, std::streampos = std::ios::beg);
	
} // namespace NParser
 #pragma once

#include <array> // std::array

#include "Debugger.hpp"

class Logger;

namespace NParser
{

//====================================================================================================================================
//=============================================================CONSTANTS==============================================================
//====================================================================================================================================

	constexpr size_t MAX_LINE_LENGTH = 1 << 8;

//====================================================================================================================================
//==============================================================CLASSES===============================================================
//====================================================================================================================================

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

//====================================================================================================================================
//========================================================FUNCTION_DECLARATION========================================================
//====================================================================================================================================

#pragma region FUNCTION_DECLARATION

	Logger &operator<<(Logger&, const Operation&);

	Operation ParseCode(std::string_view);

	bool Move2Label(std::ifstream&, std::string_view, std::streampos = std::ios::beg);

	bool Move2LabelBin(std::ifstream&, std::string_view, std::streampos = std::ios::beg);
	
#pragma endregion

} // namespace NParser
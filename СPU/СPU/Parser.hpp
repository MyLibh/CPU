 #pragma once

#include <string>

#include "MyTypedefs.hpp"

struct Operation final
{
	std::string cmd,
			    arg;

	VOID dump() const { std::cout << "OP: " << cmd << ", ARG: " << (arg.length()? arg : "NULL") << std::endl; }
};

Operation &ParseCode(CONST std::string&);
 #pragma once

#include <string>
#include <array>
#include <cassert>
#include <iomanip>
#include <iostream>

#include "MyTypedefs.hpp"

struct Operation final
{
	static CONST USHORT MAX_ARGS = 1 << 2;

	std::string cmd; // + Condition

	std::array<std::string, MAX_ARGS> args;

	explicit Operation() : cmd(), args() { }
	Operation(CONST Operation &crOp) : cmd(crOp.cmd), args(crOp.args) { }
	Operation(Operation &&rrOp) : cmd(std::move(rrOp.cmd)), args(std::move(rrOp.args)) { }
	~Operation() { }

	Operation &operator=(CONST Operation &crOp)
	{
		if (this != &crOp)
		{
			cmd  = crOp.cmd;
			args = crOp.args;
		}

		return *this;
	}

	Operation &operator=(Operation &&rrOp)
	{
		assert(this != &rrOp);

		cmd  = std::move(rrOp.cmd);
		args = std::move(rrOp.args);

		return *this;
	}

	VOID dump() const 
	{ 
		std::streamsize width = 1 << 2;

		std::cout << "OP: " << std::setw(width) << cmd;

		for (SIZE_T i = 0; i < MAX_ARGS; ++i) std::cout << ", ARG" << i << ": " << std::setw(width) << (args[i].length() ? args[i] : "NULL");

		std::cout << std::endl;
	}
}; 

Operation *ParseCode(CONST CHAR*);
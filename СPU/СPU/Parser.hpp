 #pragma once

#include <string>
#include <array>
#include <cassert>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <fstream>

#include "MyTypedefs.hpp"
#include "Debugger.hpp"

CONST SIZE_T MAX_LINE_LENGTH = 1 << 8;

struct Operation final
{
	static CONST WORD MAX_ARGS = 1 << 2;

	std::string cmd; // + cond?

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
		std::streamsize width = 1 << 3;

		NDebugger::Info("OP: ", NDebugger::TextColors::Brown, FALSE);
		std::cout << std::setw(width) << (cmd.length() ? cmd : "null");

		for (SIZE_T i = 0; i < MAX_ARGS; ++i)
		{
			std::cout << ", ";
			NDebugger::Info("ARG" + std::to_string(i + 1) + ": ", NDebugger::TextColors::Cyan, FALSE);

			std::cout << std::setw(width) << (args[i].length() ? args[i] : "null");
		}

		std::cout << std::endl;
	}
}; 

Operation *ParseCode(CONST CHAR*);

std::ifstream FindLabel(std::ifstream&, CRSTRING);
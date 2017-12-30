// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include <iostream> // std::cout
#include <iomanip>  // std::setw
#include <sstream>  // std::stringstream
#include <fstream>  // std::ifstream
#include <string>   // std::string
#include <cassert>  // assert

#include "Parser.hpp"

namespace NParser
{
	Operation::Operation() _NOEXCEPT :
		cmd(), 
		args() 
	{ }

	Operation::Operation(CONST Operation &crOp) :
		cmd(crOp.cmd), 
		args(crOp.args) 
	{ }

	Operation::Operation(Operation &&rrOp) :
		cmd(std::move(rrOp.cmd)), 
		args(std::move(rrOp.args)) 
	{ }

	Operation::~Operation() 
	{ }

	Operation &Operation::operator=(CONST Operation &crOp)
	{
		if (this != &crOp)
		{
			cmd = crOp.cmd;
			args = crOp.args;
		}

		return (*this);
	}

	Operation &Operation::operator=(Operation &&rrOp)
	{
		assert(this != &rrOp);

		cmd  = std::move(rrOp.cmd);
		args = std::move(rrOp.args);

		return (*this);
	}

	VOID Operation::dump() const
	{
		std::streamsize width = 1 << 3;

		NDebugger::Debug("OP: ", NDebugger::TextColor::LightCyan, FALSE);
		std::cout << std::setw(width) << (cmd.length() ? cmd : "null");

		for (SIZE_T i = 0; i < MAX_ARGS; ++i)
		{
			std::cout << ", ";
			NDebugger::Info("ARG" + std::to_string(i + 1) + ": ", NDebugger::TextColor::Cyan, FALSE);

			std::cout << std::setw(width) << (args[i].length() ? args[i] : "null");
		}

		std::cout << std::endl;
	}

	//===============================================================================================================================================

	static inline BOOL IsCommaExist(CRSTRING line) 
	{ 
		return (line.empty() ? FALSE : (line[line.length() - 1] == ','));
	}

	Operation *ParseCode(CONST CHAR *cpLine)
	{
		Operation *pOperation = new Operation;
		if (!pOperation) throw std::exception("[NParser::ParseCode] \"Cannot allocate memory for parsing\"\n");

		std::stringstream sstr(cpLine);
		sstr >> pOperation->cmd;

		for (auto &a : pOperation->args)
		{
			sstr >> a;
			if (IsCommaExist(const_cast<CRSTRING>(a))) a.pop_back();
		}

#ifdef DEBUG
		pOperation->dump();
#endif // DEBUG

		return pOperation;
	}

	std::ifstream FindLabel(std::ifstream &rCode, CRSTRING label)
	{
		std::ifstream code(std::move(rCode));
		if (!code.is_open()) NDebugger::Error(__FUNCTION__);

		code.seekg(0);
		while (!code.eof())
		{
			std::string tmp;
			code >> tmp;

			if (":" + label == tmp) break;
		}
		code.seekg(code.tellg());

		return code;
	}
} // namespace NParser
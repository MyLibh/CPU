// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include <iostream> // std::cout
#include <iomanip>  // std::setw
#include <sstream>  // std::stringstream
#include <fstream>  // std::ifstream
#include <string>   // std::string
#include <cassert>  // assert

#include "Parser.hpp"
#include "Logger.hpp"

extern Logger gLogger;

namespace NParser
{
#pragma region Struct Operation

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

	VOID Operation::dump(std::ostream &rOstr) const
	{
		if (!cmd.length()) return;

		std::streamsize width = 1 << 3;

		NDebugger::Info("OP: ", NDebugger::TextColor::LightCyan, FALSE, rOstr);
		rOstr << std::setw(width) << (cmd.length() ? cmd : "null");

		for (SIZE_T i = 0; i < MAX_ARGS; ++i)
		{
			if (!args[i].length()) break;

			rOstr << ", ";
			NDebugger::Info("ARG" + std::to_string(i + 1) + ": ", NDebugger::TextColor::Cyan, FALSE, rOstr);

			rOstr << std::setw(width) << (args[i].length() ? args[i] : "null");
		}

		rOstr << std::endl;
	}

	//===============================================================================================================================================
	
	Logger &operator<<(Logger &rLogger, CONST Operation &crOp)
	{
		rLogger.stdPack("Parser");

		crOp.dump(rLogger.getOfstream());

		return rLogger;
	}

#pragma endregion

	static inline BOOL IsCommaExist(CRSTRING line) 
	{ 
		return (line.empty() ? FALSE : (line[line.length() - 1] == ','));
	}

	Operation ParseCode(CONST CHAR *crLine)
	{
		Operation op;

		std::stringstream sstr(crLine);
		sstr >> op.cmd;

		for (auto &word : op.args)
		{
			sstr >> word;
			if (IsCommaExist(word)) word.pop_back();
		}

#ifdef _DEBUG
		gLogger << op;

		op.dump();
#endif // DEBUG

		return op;
	}

	BOOL Move2Label(std::ifstream &rCode, CRSTRING label, std::streampos seekFrom /* = std::ios::beg */)
	{
		if (!rCode.is_open())
		{
			NDebugger::Error(std::string("[") + __FUNCTION__ + "Ifstream is not open");

			return FALSE;
		}

		rCode.seekg(seekFrom);
		while (!rCode.eof())
		{
			std::string tmp;
			rCode >> tmp;

			if (label == tmp) break;
		}
		
		return TRUE;
	}

	BOOL Move2LabelBin(std::ifstream &rCode, CRSTRING label, std::streampos seekFrom /* = std::ios::beg */)
	{
		if (!rCode.is_open() || rCode.flags() & std::ios::binary)
		{
			NDebugger::Error(std::string("[") + __FUNCTION__ + "Ifstream is not open or open in wrong mode");

			return FALSE;
		}
		
		auto length = label.length();
		rCode.seekg(seekFrom);	
		while (!rCode.eof())
		{
			SIZE_T size = 0;
			rCode.read(reinterpret_cast<CHAR*>(&size), sizeof(size));

			//if (size + 1 == length)
			//{
				CHAR *pBuf = new CHAR[size + 1]();
				rCode.read(pBuf, size);
				pBuf[size] = '\0';

				if (label == pBuf)
				{
					delete[] pBuf;

					break;
				}

				delete[] pBuf;
			//}
			//else rCode.seekg(std::ios::cur + static_cast<std::streampos>(size));
		}

		return TRUE;
	}
	
} // namespace NParser
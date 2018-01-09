// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include <iostream> // std::cout
#include <iomanip>  // std::setw
#include <sstream>  // std::stringstream
#include <fstream>  // std::ifstream
#include <string>   // std::to_string
#include <cassert>  // assert
#include <memory>   // std::unique_ptr

#include "Parser.hpp"
#include "Logger.hpp"

namespace NParser
{
#pragma region Struct Operation

	Operation &Operation::operator=(const Operation &crOp) noexcept
	{
		if (this != &crOp)
		{
			cmd = crOp.cmd;
			args = crOp.args;
		}

		return (*this);
	}

	Operation &Operation::operator=(Operation &&rrOp) noexcept
	{
		assert(this != &rrOp);

		cmd  = std::move(rrOp.cmd);
		args = std::move(rrOp.args);

		return (*this);
	}

	void Operation::dump(std::ostream &rOstr) const
	{
		if (!cmd.length()) return;

		std::streamsize width = 1 << 3;

		NDebugger::Info("OP: ", NDebugger::TextColor::LightCyan, false, rOstr);
		rOstr << std::setw(width) << (cmd.length() ? cmd : "null");

		for (size_t i = 0; i < MAX_ARGS; ++i)
		{
			if (!args[i].length()) break;

			rOstr << ", ";

			NDebugger::Info("ARG" + std::to_string(i + 1) + ": ", NDebugger::TextColor::Cyan, false, rOstr);

			rOstr << std::setw(width) << (args[i].length() ? args[i] : "null");
		}

		rOstr << std::endl;
	}

	//===============================================================================================================================================
	
	Logger &operator<<(Logger &rLogger, const Operation &crOp)
	{
		if(crOp.cmd.length()) Logger::stdPack("Parser");

		crOp.dump(Logger::getOfstream());

		return rLogger;
	}

#pragma endregion

	static inline bool IsCommaExist(std::string_view line) 
	{ 
		return (line.empty() ? false : (line.back() == ','));
	}

	Operation ParseCode(std::string_view line)
	{
		Operation op;

		std::stringstream sstr(line.data());
		sstr >> op.cmd;

		for (auto&& word : op.args)
		{
			sstr >> word;
			if (IsCommaExist(word)) word.pop_back();
		}

		if (op.cmd.length()) Logger::stdPack("Parser");
		op.dump(Logger::getOfstream());

#ifdef _DEBUG
		op.dump();
#endif // DEBUG

		return op;
	}

	bool Move2Label(std::ifstream &rCode, std::string_view label, std::streampos startFrom /* = std::ios::beg */)
	{
		if (!rCode.is_open())
		{
			NDebugger::Error(std::string("[") + __FUNCTION__ + "Ifstream is not open\n");

			return false;
		}

		rCode.seekg(startFrom);
		while (!rCode.eof())
		{
			std::string tmp;
			rCode >> tmp;

			if (label == tmp) break;
		}
		
		return true;
	}

	bool Move2LabelBin(std::ifstream &rCode, std::string_view label, std::streampos startFrom /* = std::ios::beg */)
	{
		if (!rCode.is_open())
		{
			NDebugger::Error(std::string("[") + __FUNCTION__ + "] Ifstream is not open\n");

			return false;
		}

		if (rCode.flags() & std::ios::binary)
		{
			NDebugger::Error(std::string("[") + __FUNCTION__ + "] Ifstream is open in wrong mode\n");

			return false;
		}
		
		auto length = label.length(); 
		rCode.seekg(startFrom);
		while (!rCode.eof())
		{
			size_t size = 0;
			rCode.read(reinterpret_cast<char*>(&size), sizeof(size));

			if (size == length)
			{
				std::unique_ptr<char[]> buf(new char[size + 1]);
				rCode.read(buf.get(), size);
				buf[size] = '\0';
		
				if (label == buf.get()) break;
			}
			else rCode.seekg(rCode.tellg() + static_cast<std::streampos>(size));
		}

		return true;
	}
	
} // namespace NParser
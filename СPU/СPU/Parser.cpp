// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include <iostream>
#include <sstream>

#include "Parser.hpp"

static inline BOOL IsCommaExist(CONST std::string &crLine) { return (crLine.empty()? FALSE : (crLine[crLine.length() - 1] == ',')); }

Operation *ParseCode(CONST CHAR *cpLine)
{
	Operation *pOperation = nullptr;
	try
	{
		pOperation = new Operation;
	}
	catch (CONST std::bad_alloc &crBadAlloc)
	{
		std::cerr << crBadAlloc.what();

		return (pOperation = nullptr);
	}
	catch (...)
	{
		std::cerr << "Unhandled exception";

		return (pOperation = nullptr);
	}

	std::stringstream sstr(cpLine);
	sstr >> pOperation->cmd;

	for (auto &a : pOperation->args)
	{
		sstr >> a;
		if (IsCommaExist(const_cast<CONST std::string&>(a))) a.pop_back();
	}

#ifdef DEBUG
	pOperation->dump();
#endif // DEBUG

	return pOperation;
}
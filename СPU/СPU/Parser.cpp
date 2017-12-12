#include <iostream>
#include <sstream>

#include "Parser.hpp"

Operation &ParseCode(CONST std::string &crFilename)
{
	Operation *pOperation = nullptr;
	try
	{
		pOperation = new Operation;
	}
	catch (CONST std::bad_alloc &crBadAlloc)
	{
		std::cerr << crBadAlloc.what();

		return *(pOperation = nullptr);
	}
	catch (...)
	{
		std::cerr << "Unhandled exception";

		return *(pOperation = nullptr);
	}

	std::stringstream sstr(crFilename);
	sstr >> pOperation->cmd;
	sstr >> pOperation->arg;

	pOperation->dump();

	return *pOperation;
}
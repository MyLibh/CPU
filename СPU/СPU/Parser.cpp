// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "Parser.hpp"

static inline BOOL IsCommaExist(CRSTRING line) { return (line.empty()? FALSE : (line[line.length() - 1] == ',')); }

Operation *ParseCode(CONST CHAR *cpLine)
{
	Operation *pOperation = nullptr;
	try
	{
		pOperation = new Operation;
	}
	catch (CONST std::bad_alloc &crBadAlloc)
	{
		NDebugger::Error(crBadAlloc.what());

		return (pOperation = nullptr);
	}
	catch (...)
	{
		NDebugger::Error("Unhandled exception");

		return (pOperation = nullptr);
	}

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
	while(!code.eof())
	{
		std::string tmp;
		code >> tmp;

		if (":" + label == tmp) break;
	}
	code.seekg(code.tellg());

	return code;
}

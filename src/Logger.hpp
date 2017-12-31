#pragma once

#include <iosfwd> // std::ofstream

#include "Debugger.hpp"

class Logger
{
public:
	enum Type
	{
		Debug,
		Release
	};

	Logger();
	~Logger();

	std::ofstream &getOfstream();

	//template<typename T>
	//BOOL saveData(CONST T&);

	BOOL stdPack(CRSTRING, Type = Type::Debug);
	BOOL write(CRSTRING, CRSTRING, Type = Type::Debug);

private:
	std::ofstream log_;
};

#define LOG_DUMPING()      gLogger << *this;
#define LOG_CONSTRUCTING() gLogger.write(__func__, "Constructing");
#define LOG_DESTRUCTING()  gLogger.write(__func__, "Destructing");

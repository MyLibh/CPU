#pragma once

#include <iosfwd> // std::ofstream

#include "Debugger.hpp"

class Logger
{
public:
	enum class Type
	{
		Debug,
		Release
	};

	Logger();
	~Logger();

	std::ofstream &getOfstream();

	template<typename T>
	VOID printData(SIZE_T, ...);

	BOOL stdPack(CRSTRING, Type = Type::Debug);
	BOOL write(CRSTRING, CRSTRING, Type = Type::Debug);

private:
	std::ofstream log_;
};

//===============================================================================================================================================

template<typename T>
Logger &operator<<(Logger&, CONST T&);

template<typename T>
inline Logger &operator<<(Logger &rLog, CONST T &crVal)
{
	rLog.log_ << crVal;

	return rLog;
}

//===============================================================================================================================================

extern Logger gLogger;

#ifdef _DEBUG
	#define LOG_DUMPING()       gLogger << *this;
	#define LOG_CONSTRUCTING()  gLogger.write(__FUNCTION__, "Constructing");
	#define LOG_DESTRUCTING()   gLogger.write(__FUNCTION__, "Destructing");
	#define LOG_ERROR(error)    gLogger.write(__FUNCTION__ + std::string("\t[ERROR]"), error);
	#define LOG_INFO(argc, ...) gLogger.printData(argc, __VA_ARGS__);
#else
	#define LOG_DUMPING()      
	#define LOG_CONSTRUCTING()  
	#define LOG_DESTRUCTING()
	#define LOG_ERROR(error)   gLogger.write(__FUNCTION__ + std::string("\t[ERROR]"), error);
	#define LOG_INFO()         
#endif // _DEBUG


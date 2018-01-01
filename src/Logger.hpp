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

#ifdef _DEBUG
	#define LOG_DUMPING()      gLogger << *this;
	#define LOG_CONSTRUCTING() gLogger.write(__FUNCTION__, "Constructing");
	#define LOG_DESTRUCTING()  gLogger.write(__FUNCTION__, "Destructing");
	#define LOG_ERROR(error)   gLogger.write(__FUNCTION__, error);
	#define LOG_INFO()
#elif defined RELEASE
	#define LOG_DUMPING()      
	#define LOG_CONSTRUCTING() gLogger.write(__FUNCTION__, "Constructing");	
	#define LOG_DESTRUCTING()  gLogger.write(__FUNCTION__, "Destructing");  
	#define LOG_ERROR(error) 
	#define LOG_INFO()
#else
	#define LOG_DUMPING()      
	#define LOG_CONSTRUCTING()  
	#define LOG_DESTRUCTING()
	#define LOG_ERROR(error)
#endif // _DEBUG
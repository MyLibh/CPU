#pragma once

#include <iosfwd>  // std::ofstream
#include <vector> 

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

	template<typename ...T>
	VOID printData(CRSTRING func, CONST T &...data)
	{
		stdPack(func);

		using dest_t = typename std::common_type<T...>::type;
		std::vector<dest_t> vec{ static_cast<dest_t>(data)... };

		auto size = vec.size();
		for (SIZE_T i = 0; i < size; ++i)
		{
			
			log_ << "ARG" << i + 1 << ": " << vec[i] << (i + 1 == size ? "\n" : ", ");
		}
	}

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
	#define LOG_DUMP()          gLogger << *this;
	#define LOG_CONSTRUCTING()  gLogger.write(__FUNCTION__, "Constructing");
	#define LOG_DESTRUCTING()   gLogger.write(__FUNCTION__, "Destructing");
	#define LOG_ERROR(error)    gLogger.write(__FUNCTION__ + std::string("\t[ERROR]"), error);
	#define LOG_ARGS(type, ...) gLogger.printData<type>(__FUNCTION__, __VA_ARGS__);
	#define LOG_FUNC()          gLogger.write(__FUNCTION__, "");
#else
	#define LOG_DUMPING()      
	#define LOG_CONSTRUCTING()  
	#define LOG_DESTRUCTING()
	#define LOG_ERROR(error)   gLogger.write(__FUNCTION__ + std::string("\t[ERROR]"), error);
	#define LOG_ARGS()      
	#define LOG_FUNC()
#endif // _DEBUG


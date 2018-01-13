#pragma once

#include <iosfwd>      // std::ofstream
#include <vector>      // std::vector
#include <string_view> // std::string_view

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

	static bool init();
	static bool close();

	static std::ofstream &getOfstream();

	template<typename... T>
	static void printData(std::string_view func, const T &...data)
	{
		stdPack(func);

		std::vector<typename std::common_type<T...>::type> vec{ static_cast<typename std::common_type<T...>::type>(data)... };

		auto size = vec.size();
		for (size_t i = 0; i < size; ++i)
		{		
			log_ << "ARG" << i + 1 << ": " /* << vec[i] */ << (i + 1 == size ? "\n" : ", ");
		}
	}

	static bool stdPack(std::string_view, Type = Type::Debug);
	static bool write(std::string_view, std::string_view, Type = Type::Debug);

private:
	static std::ofstream log_;
	static bool          init_;
};

//====================================================================================================================================
//========================================================FUNCTION_DECLARATION========================================================
//====================================================================================================================================

template<typename T>
inline Logger &operator<<(Logger&, const T&);

//====================================================================================================================================
//========================================================FUNCTION_DEFINITION=========================================================
//====================================================================================================================================

template<typename T>
inline Logger &operator<<(Logger &rLog, const T &crVal)
{
	rLog.log_ << crVal;

	return rLog;
}

//====================================================================================================================================
//==============================================================DEFINES===============================================================
//====================================================================================================================================

#ifdef _DEBUG
	//#define LOG_DUMP()          Logger() << *this;
	#define LOG_CONSTRUCTING()  Logger::write(__FUNCTION__, "Constructing");
	#define LOG_DESTRUCTING()   Logger::write(__FUNCTION__, "Destructing");
	#define LOG_ERROR(error)    Logger::write(__FUNCTION__ + "\t[ERROR]", error);
	#define LOG_ARGS(type, ...) Logger::printData<type>(__FUNCTION__, __VA_ARGS__);
	#define LOG_FUNC()          Logger::write(__FUNCTION__, "");
#else
	#define LOG_DUMP()      
	#define LOG_CONSTRUCTING()  
	#define LOG_DESTRUCTING()
	#define LOG_ERROR(error)    Logger::write(__FUNCTION__ + std::string("\t[ERROR]"), error);
	#define LOG_ARGS()      
	#define LOG_FUNC()
#endif // _DEBUG


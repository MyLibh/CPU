// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include <fstream> // std::ofstream
#include <iomanip> // std::setw, std::put_time
#include <ctime>   // std::time_t, std::tm
#include <chrono>  // std::chrono::system_clock

#include "Logger.hpp"

Logger gLogger; // Global instance of class Logger for all other classes

CONST std::streamsize LOG_FUNC_SIZE = 1 << 6;

inline Logger::Logger() :
	log_("CPU.log")
{
	if (!log_.is_open()) throw std::runtime_error("Cannot open build file\n");
	
	log_ << "[DEBUG][" << __TIME__ << "][" << std::setw(LOG_FUNC_SIZE) << "Logger" << "] Logging started\n";
}

inline Logger::~Logger()
{
	write("Logger", "Logging finished");

	log_.close();
}

std::ofstream &Logger::getOfstream()
{
	return log_;
}

BOOL Logger::stdPack(CRSTRING func, Type type /* = Type::Debug */)
{
	if (!log_.is_open()) return FALSE;

	using std::chrono::system_clock;
	auto tt = system_clock::to_time_t(system_clock::now());

	std::tm *ptm = new tm;
	localtime_s(ptm, &tt);

	log_ << "[" << (type == Type::Debug ? "DEBUG]" : "RELEASE]") << "["
		 << std::put_time(ptm, "%X")
		 << "][" << std::setw(LOG_FUNC_SIZE) << func.c_str() << "] ";

	delete ptm;

	return TRUE;
}

inline BOOL Logger::write(CRSTRING func, CRSTRING info, Type type /* = Type::Debug */)
{
	stdPack(func, type);

	log_ << info.c_str() << std::endl;

	return TRUE;
}

//===============================================================================================================================================

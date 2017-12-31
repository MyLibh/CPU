// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include <fstream> // std::ofstream
#include <iomanip> // std::setw

#include "Logger.hpp"

Logger gLogger; // Global instance of class Logger for all other classes

Logger::Logger() :
	log_("Log")
{
	if (!log_.is_open()) throw std::runtime_error("Cannot open build file\n");
	
	write("Logger", "Logging started");
}

Logger::~Logger()
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

	log_ << "[" << (type == Type::Debug ? "DEBUG]" : "RELEASE]") << "[" << __TIME__ << "][" << std::setw(10) << func.c_str() << "] ";

	return TRUE;
}

BOOL Logger::write(CRSTRING func, CRSTRING info, Type type /* = Type::Debug */)
{
	stdPack(func, type);

	log_ << info.c_str() << std::endl;

	return TRUE;
}
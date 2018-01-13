// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include <fstream> // std::ofstream
#include <iomanip> // std::setw, std::put_time
#include <ctime>   // std::time_t, std::tm
#include <chrono>  // std::chrono::system_clock

#include "Logger.hpp"

#pragma region CONSTANTS

constexpr std::streamsize LOG_FUNC_SIZE = 1 << 6;

#pragma endregion

//====================================================================================================================================
//==========================================================STATIC_VARIABLES==========================================================
//====================================================================================================================================

#pragma region STATIC_VARIABLES

std::ofstream Logger::log_;

bool Logger::init_ = false;

#pragma endregion

//====================================================================================================================================
//=========================================================METHOD_DEFINITION==========================================================
//====================================================================================================================================

#pragma region METHOD_DEFINITION

Logger::Logger() 
{
	Logger::init();
}

bool Logger::init()
{
	if (!init_)
	{
		init_ = true;

		log_.open("CPU.log");
	}
	else return log_.is_open();

	if (!log_.is_open()) throw std::runtime_error("Cannot open log file\n");

	log_ << "[DEBUG][" << __TIME__ << "][" << std::setw(LOG_FUNC_SIZE) << "Logger" << "] Logging started\n";

	return true;
}

bool Logger::close()
{
	if (!init_) return false;
	
	init_ = false;
	write("Logger", "Logging finished");

	log_.close();

	return true;
}

std::ofstream &Logger::getOfstream()
{
	return log_;
}

bool Logger::stdPack(std::string_view func, Type type /* = Type::Debug */)
{
	if (!log_.is_open()) return false;

	using std::chrono::system_clock;
	auto tt = system_clock::to_time_t(system_clock::now());

	std::tm *ptm = new tm;
	localtime_s(ptm, &tt);

	log_ << "[" << (type == Type::Debug ? "DEBUG]" : "RELEASE]") << "["
		 << std::put_time(ptm, "%X")
		 << "][" << std::setw(LOG_FUNC_SIZE) << func.data() << "] ";

	delete ptm;

	return true;
}

inline bool Logger::write(std::string_view func, std::string_view info, Type type /* = Type::Debug */)
{
	if(!stdPack(func, type)) return false;

	log_ << info.data() << std::endl;

	return true;
}

#pragma endregion
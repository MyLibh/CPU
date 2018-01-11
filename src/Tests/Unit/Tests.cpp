// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#define USE_UNIT_TESTS

#define STORAGE_TESTS

#ifdef USE_UNIT_TESTS

#include <cassert>  // assert
#include <iostream> // std::cerr

#define UNREACHABLE_PLACE std::cerr << "[ERROR] In function: " << __FUNCTION__ << ", on line: " << __LINE__ << std::endl;

#ifdef STORAGE_TESTS

#include "../../Storage.hpp"

void operator_OORT_failure()
{
	Storage<int, 10> s;

	s[11] = 0;

	UNREACHABLE_PLACE
}

void operator_OORD_failure()
{
	Storage<int, 10> s;

	s[-1] = 0;

	UNREACHABLE_PLACE
}

#endif // STORAGE_TESTS

#endif // USE_UNIT_TESTS


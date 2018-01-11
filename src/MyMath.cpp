// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include <cmath> 

#include "MyMath.hpp"

//====================================================================================================================================
//========================================================FUNCTION_DEFINITION=========================================================
//====================================================================================================================================

double      sqrt_(double      x) { return std::sqrt (x); }
long double sqrt_(long double x) { return std::sqrtl(x); }
float       sqrt_(float       x) { return std::sqrtf(x); }

double      sin_(double      x) { return std::sin (x); }
long double sin_(long double x) { return std::sinl(x); }
float       sin_(float       x) { return std::sinf(x); }

double      cos_(double      x) { return std::cos (x); }
long double cos_(long double x) { return std::cosl(x); }
float       cos_(float       x) { return std::cosf(x); }

double      abs_(double      x) { return std::fabs (x); }
long double abs_(long double x) { return std::fabsl(x); }
float       abs_(float       x) { return std::fabsf(x); }
long        abs_(long        x) { return std::labs (x); }
long long   abs_(long long   x) { return std::llabs(x); }
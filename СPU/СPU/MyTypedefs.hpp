#pragma once

#ifndef _WINDOWS_
	
	typedef bool		   BOOL;
	typedef int			   INT;
	typedef size_t         SIZE_T;
	typedef char           CHAR;
	typedef unsigned short WORD;

	#define CONST const

	#define TRUE  true
	#define FALSE false

#endif // _WINDOWS

#ifdef VOID
	#undef VOID
#endif // VOID

typedef void     VOID;
typedef float    FLOAT;
typedef double   DOUBLE;
typedef unsigned UNSIGNED;

#ifdef _CRT_SECURE_CPP_OVERLOAD_STANDART_NAMES
	#undef _CRT_SECURE_CPP_OVERLOAD_STANDART_NAMES
#endif // _CRT_SECURE_CPP_OVERLOAD_STANDART_NAMES

#ifdef _CRT_SECURE_NO_WARNINGS
	#undef _CRT_SECURE_NO_WARNINGS
#endif // _CRT_SECURE_NO_WARNINGS

#ifdef _CRT_SECURE_NO_WARNINGS
	#undef _CRT_SECURE_NO_WARNINGS
#endif // _CRT_SECURE_NO_WARNINGS

#define _CRT_SECURE_CPP_OVERLOAD_STANDART_NAMES 0
#define _CRT_SECURE_NO_WARNINGS                 0
#define _CRT_NONSTDC_NO_WARNINGS                0

#include <string>
typedef CONST std::string &CRSTRING;

//===============================================================================================================================================

#define DEBUG
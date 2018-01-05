#pragma once

#if _MSVC_LANG < 201402
	namespace std
	{
		template<typename T>
		class _Is_nothrow_swappable 
		{
			/* constexpr */ operator bool() { return false; }
		};
	}

	#define _NOEXCEPTARG(arg)	
	#define _CONSTEXPR17 inline
#else
	#define _NOEXCEPTARG(...) _NOEXCEPT(__VA_ARGS__) 
#endif // _MSVC_LANG 

#ifndef _WINDOWS_
	
	typedef bool		   BOOL;   //-V677
	typedef int			   INT;    //-V677
	typedef size_t         SIZE_T; //-V677
	typedef char           CHAR;   //-V677
	typedef unsigned short WORD;   //-V677
	typedef unsigned int   DWORD;  //-V677

	#define CONST const

	#define TRUE  true
	#define FALSE false

#endif // _WINDOWS

#ifdef VOID
	#undef VOID
#endif // VOID

typedef void     VOID; //-V677
typedef float    FLOAT; //-V677
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

class string;
typedef CONST std::string &CRSTRING;

//===============================================================================================================================================

#define DEBUG
#pragma once

#include "Hash.hpp"

#if   CPU_GUARD_LVL >= 2
	#define   HASH_GUARD(...) __VA_ARGS__
	#define CANARY_GUARD(...) __VA_ARGS__
	#define  GUARD_CHECK(   ) if(!this->ok()) std::cerr << "[ERROR] "<< __FUNCTION__ << std::endl, this->dump();
#elif CPU_GUARD_LVL == 1
	#define   HASH_GUARD(...) 
	#define CANARY_GUARD(...) __VA_ARGS__
	#define  GUARD_CHECK(   ) if(!this->ok()) std::cerr << "[ERROR] "<< __FUNCTION__ << std::endl, this->dump();
#else
	#define   HASH_GUARD(...)
	#define CANARY_GUARD(...) 
	#define  GUARD_CHECK(   )
#endif // CPU_GUARD_LVL



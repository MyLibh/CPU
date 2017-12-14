#pragma once

#include <fstream>
#include <string>
#include <cassert>
#include <type_traits>
#include <array>

#include "Stack.hpp"
#include "MyMath.hpp"

//cmd -D_SCL_SECURE_NO_WARNINGS

template<typename T = INT>
struct Register final
{
	enum REGISTERS : SIZE_T
	{
		AX,
		BX,
		CX,
		DX,
		EX,
		//SP,

		NUM
	};

	std::array<T, REGISTERS::NUM> regs;

	explicit Register() : regs() { }
	Register(CONST Register<T> &crReg) : regs(crReg.regs) { }
	Register(Register<T> &&rrReg) : regs(std::move(rrReg.regs)) { }
	~Register() { }

	Register<T> &operator=(CONST Register<T> &crReg)
	{
		if (this != &crReg) regs = crReg.regs;
		
		return *this;
	}

	Register<T> &operator=(Register<T> &&rrReg)
	{
		assert(this != &rrReg);

		regs = std::move(rrReg.regs);
	
		return *this;
	}

	VOID swap(Register<T> &rReg) { reg.swap(rReg.reg); }

	VOID dump() const
	{
		std::cout << "[REGISTER]\n";

		for (SIZE_T i = 0; i < REGISTERS::NUM; ++i) std::cout << static_cast<CHAR>('A' + i) << "X: " << regs[i] << std::endl;

		std::cout << std::endl;
	}
};

//==================================================================================================================

template<typename T = INT>
class CPU final
{	
	Register<T>   reg_;
	Stack<T>      stack_;

public:
	explicit CPU() : 
		reg_(), 
		stack_() 
	{ }

	CPU(CONST CPU<T> &crCPU) : 
		reg_(crCPU.reg_),
		stack_(crCPU.stack_) 
	{ }

	CPU(CPU<T> &&rrProc) : 
		reg_(), 
		stack_() 
	{ *this = std::move(rrProc); }

	~CPU() { }

	CPU<T> &operator=(CONST CPU<T>&);
	CPU<T> &operator=(CPU<T>&&);
	
	VOID   pop()                { stack_.pop(); } 
	SIZE_T getStackSize() const { return stack_.size(); }
	VOID   push(CONST T &crVal) { stack_.push(crVal); }
	
	BOOL add();
	BOOL sub();
	BOOL mul();
	BOOL div();
	BOOL dup();

	BOOL sqrt();
	BOOL sin();
	BOOL cos();

	VOID swap(CPU<T> &rCPU) { stack_.swap(rCPU.stack_); reg_.swap(rCPU.reg_); }


	VOID dump() const 
	{ 
		std::cout << "\n\t\t[CPU DUMP]\n";

		reg_.dump(); 
		stack_.dump(); 

		std::cout << "\n\t\t[  END   ]\n";
	}
};

#define CHECK_STACK_SIZEN(size)                                                                                         \
		if(stack_.size() <= size)                                                                                       \
		{                                                                                                               \
			std::cerr << "There are not enough elements in stack, number of elements = " << stack_.size() << std::endl; \
																													    \
			return FALSE;                                                                                               \
		}

#define CHECK_STACK_SIZE()                                                                                              \
		if(!stack_.size())                                                                                              \
		{                                                                                                               \
			std::cerr << "There are not enough elements in stack, number of elements = " << stack_.size() << std::endl; \
																													    \
			return FALSE;                                                                                               \
		}

template<typename T>
CPU<T> &CPU<T>::operator=(CONST CPU<T> &crCPU)
{
	if(this != &crCPU)
	{
		reg_   = crCPU.reg_;
		stack_ = crCPU.stack_;
	}

	return *this;
}

template<typename T>
CPU<T> &CPU<T>::operator=(CPU<T> &&rrCPU)
{
	assert(this != &rrCPU);
	
	reg_   = std::move(rrCPU.reg_);
	stack_ = std::move(rrCPU.stack_);
	
	return *this;
}

template<typename T>
BOOL CPU<T>::add()
{
	CHECK_STACK_SIZEN(1)

	auto a = stack_.pop();
	auto b = stack_.pop();
	
	stack_.push(a + b);

	return TRUE;
}

template<typename T>
BOOL CPU<T>::sub()
{
	CHECK_STACK_SIZEN(1)

	auto a = stack_.pop();
	auto b = stack_.pop();

	stack_.push(a - b);

	return TRUE;
}

template<typename T>
BOOL CPU<T>::mul()
{
	CHECK_STACK_SIZEN(1)

	auto a = stack_.pop();
	auto b = stack_.pop();

	stack_.push(a * b);

	return TRUE;
}

template<typename T>
BOOL CPU<T>::div()
{
	CHECK_STACK_SIZEN(1)

	auto a = stack_.pop();
	auto b = stack_.pop();

 	assert(b);
	stack_.push(a / b);

	return TRUE;
}

template<typename T>
BOOL CPU<T>::dup()
{
	CHECK_STACK_SIZE()

	auto a = stack_.pop();

	stack_.push(a);
	stack_.push(a);

	return TRUE;
}

template<typename T>
BOOL CPU<T>::sqrt()
{
	CHECK_STACK_SIZE()

	static_assert(std::is_arithmetic<T>::value, "Type T must be arithmetic\n");

	auto a = stack_.pop();

	stack_.push(static_cast<T>(sqrt_(std::is_integral<T>::value? static_cast<DOUBLE>(a) : a)));

	return TRUE;
}

template<typename T>
BOOL CPU<T>::sin()
{
	CHECK_STACK_SIZE()

	static_assert(std::is_arithmetic<T>::value, "Type T must be arithmetic\n");

	auto a = stack_.pop();

	stack_.push(static_cast<T>(sin_(std::is_integral<T>::value? static_cast<DOUBLE>(a) : a)));

	return TRUE;
}

template<typename T>
BOOL CPU<T>::cos()
{
	CHECK_STACK_SIZE()

	static_assert(std::is_arithmetic<T>::value, "Type T must be arithmetic\n");

	auto a = stack_.pop();

	stack_.push(static_cast<T>(cos_(std::is_integral<T>::value? static_cast<DOUBLE>(a) : a)));

	return TRUE;
}

#undef CHECK_STACK_SIZE
#undef CHECK_STACK_SIZEN


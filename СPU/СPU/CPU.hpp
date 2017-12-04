#pragma once

#include <fstream>
#include <string>
#include <cassert>
#include <type_traits>

#include "Stack.hpp"
#include "MyMath.hpp"

//cmd -D_SCL_SECURE_NO_WARNINGS

template<typename T = INT>
class CPU final
{
	Stack<T> stack_;

public:
	enum commands_
	{
		// Stack commands
		_push,
		_pop,
		_add,
		_sub,
		_mul,
		_div,
		_sqrt,
		_dup, 
		_sin, 
		_cos,

		// CPU commands
		jump,
		je,
		jne,
		ja,
		jae,
		jb,
		jbe
	};

	explicit CPU() : stack_() { }
	CPU(CONST CPU<T> &crCPU) : stack_(crCPU.stack_) { }
	CPU(CPU<T> &&rrProc) : stack_() { *this = std::move(rrProc); }
	~CPU() { }

	CPU<T> &operator=(CONST CPU<T>&);
	CPU<T> &operator=(CPU<T>&&);

	T      pop()                { return stack_.pop(); }
	SIZE_T getStackSize() const { return stack_.size(); }
	VOID   push(CONST T &crVal) { stack_.push(crVal); }
	VOID   dumpStack() const    { stack_.dump(); }

	BOOL add();
	BOOL sub();
	BOOL mul();
	BOOL div();
	BOOL dup();

	BOOL sqrt();
	BOOL sin();
	BOOL cos();

	VOID swap(CPU<T> &rCPU) { stack_.swap(rCPU.stack_); } 

	BOOL compileFromFile(CONST std::string&);
};

template<typename T>
CPU<T> &CPU<T>::operator=(CONST CPU<T> &crCPU)
{
	if(this != &crCPU)
	{
		stack_ = crCPU.stack_;
	}

	return *this;
}

template<typename T>
CPU<T> &CPU<T>::operator=(CPU<T> &&rrCPU)
{
	assert(this != &rrCPU);
	
	stack_ = std::move(rrCPU.stack_);
	
	return *this;
}

template<typename T>
BOOL CPU<T>::add()
{
	if(stack_.size() <= 1)
	{
		std::cerr << "There are not enough elements in stack, number of elements = " << stack_.size() << std::endl;

		return FALSE;
	}

	auto a = stack_.pop();
	auto b = stack_.pop();
	
	stack_.push(a + b);

	return TRUE;
}

template<typename T>
BOOL CPU<T>::sub()
{
	if(stack_.size() <= 1)
	{
		std::cerr << "There are not enough elements in stack, number of elements = " << stack_.size() << std::endl;

		return FALSE;
	}

	auto a = stack_.pop();
	auto b = stack_.pop();

	stack_.push(a - b);

	return TRUE;
}

template<typename T>
BOOL CPU<T>::mul()
{
	if(stack_.size() <= 1)
	{
		std::cerr << "There are not enough elements in stack, number of elements = " << stack_.size() << std::endl;

		return FALSE;
	}

	auto a = stack_.pop();
	auto b = stack_.pop();

	stack_.push(a * b);

	return TRUE;
}

template<typename T>
BOOL CPU<T>::div()
{
	if(stack_.size() <= 1)
	{
		std::cerr << "There are not enough elements in stack, number of elements = " << stack_.size() << std::endl;

		return FALSE;
	}

	auto a = stack_.pop();
	auto b = stack_.pop();

 	assert(a);
	stack_.push(b / a);

	return TRUE;
}

template<typename T>
BOOL CPU<T>::dup()
{
	if(!stack_.size())
	{
		std::cerr << "There are not enough elements in stack, number of elements = " << stack_.size() << std::endl;

		return FALSE;
	}

	auto a = stack_.pop();

	stack_.push(a);
	stack_.push(a);

	return TRUE;
}

template<typename T>
BOOL CPU<T>::sqrt()
{
	if(!stack_.size())
	{
		std::cerr << "There are not enough elements in stack, number of elements = " << stack_.size() << std::endl;

		return FALSE;
	}

	static_assert(std::is_arithmetic<T>::value, "Type T must be arithmetic\n");

	auto a = stack_.pop();

	stack_.push(static_cast<T>(sqrt_(std::is_integral<T>::value? static_cast<DOUBLE>(a) : a)));

	return TRUE;
}

template<typename T>
BOOL CPU<T>::sin()
{
	if(!stack_.size())
	{
		std::cerr << "There are not enough elements in stack, number of elements = " << stack_.size() << std::endl;

		return FALSE;
	}

	static_assert(std::is_arithmetic<T>::value, "Type T must be arithmetic\n");

	auto a = stack_.pop();

	stack_.push(static_cast<T>(sin_(std::is_integral<T>::value? static_cast<DOUBLE>(a) : a)));

	return TRUE;
}

template<typename T>
BOOL CPU<T>::cos()
{
	if(!stack_.size())
	{
		std::cerr << "There are not enough elements in stack, number of elements = " << stack_.size() << std::endl;

		return FALSE;
	}

	static_assert(std::is_arithmetic<T>::value, "Type T must be arithmetic\n");

	auto a = stack_.pop();

	stack_.push(static_cast<T>(cos_(std::is_integral<T>::value? static_cast<DOUBLE>(a) : a)));

	return TRUE;
}

template<typename T>
BOOL CPU<T>::compileFromFile(CONST std::string &crFilename)
{
	std::ifstream file(crFilename);
	if(!file.is_open())
	{
		std::cerr << "Cannot open file: " << crFilename << std::endl;

		return FALSE;
	}

	while(!file.eof())
	{
		std::string tmp;
		file >> tmp;

		if(tmp == "push")
		{
			T val = NULL;
			file >> val;

			stack_.push(val);
		}
		else if(tmp == "pop") pop();
		else if(tmp == "add") add();
		else if(tmp == "sub") sub();
		else if(tmp == "mul") mul();
		else if(tmp == "div") div();
		else if(tmp == "dup") dup();		
		else if(tmp == "sin") sin();
		else if(tmp == "cos") cos();
		else if(tmp == "sqrt") sqrt();
		else if(tmp == "dumpStack") dumpStack();
		else { std::cerr << "Unknown command: " << tmp << std::endl; return FALSE; }
	}
	file.close();

	return TRUE;
} 
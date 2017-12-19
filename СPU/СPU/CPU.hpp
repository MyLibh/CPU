#pragma once

#include <fstream>
#include <string>
#include <cassert>
#include <type_traits>

#include "Register.hpp"
#include "RAM.hpp"
#include "Stack.hpp"
#include "MyMath.hpp"

//cmd -D_SCL_SECURE_NO_WARNINGS

namespace NCPU
{
#pragma region Usings

	using NReg::REG;
	using NReg::Register;

	using NRAM::RAM;

	using NStack::Stack;

#pragma endregion

	template<typename T = INT>
	class CPU final
	{
		Register<T> reg_;
		RAM<T>      ram_;
		Stack<T>    stack_;

		BOOL CheckStackSize(SIZE_T = 0) const;

	public:
		typedef       T  &rVal_;
		typedef       T &&rrVal_;
		typedef CONST T  &crVal_;

		explicit CPU();
		CPU(CONST CPU<T>&);
		CPU(CPU<T>&&);
		~CPU();

		CPU<T> &operator=(CONST CPU<T>&);
		CPU<T> &operator=(CPU<T>&&);

		VOID push(crVal_);
		VOID push(rrVal_);
		VOID push(REG);
		VOID pop();

		//VOID pushram(crVal_);
		//VOID pushram(rrVal_);
		//VOID pushram(REG);
		//VOID popram();

		BOOL add();
		BOOL sub();
		BOOL mul();
		BOOL div();
		BOOL dup();

		BOOL sqrt();
		BOOL sin();
		BOOL cos();

		std::pair<T, T> getPair();

		VOID swap(CPU<T>&);

		VOID move(REG, REG);
		VOID move(crVal_, REG);

		//VOID move(REG src, rVal_ dest)   const { dest = reg_[src]; }
		//VOID move(crVal_src, rVal_ dest) const { dest = src; }

		VOID dump() const;
	};

	template<typename T>
	CPU<T>::CPU() :
		reg_(),
		ram_(),
		stack_()
	{ }

	template<typename T>
	CPU<T>::CPU(CONST CPU<T> &crCPU) :
		reg_(crCPU.reg_),
		ram_(crCPU.ram_),
		stack_(crCPU.stack_)
	{ }

	template<typename T>
	CPU<T>::CPU(CPU<T> &&rrCPU) :
		reg_(std::move(rrCPU.reg_)),
		ram_(std::move(rrCPU.ram_)),
		stack_(std::move(rrCPU.stack_))
	{ }

	template<typename T>
	CPU<T>::~CPU()
	{ }

	template<typename T>
	BOOL CPU<T>::CheckStackSize(SIZE_T size /* = 0 */) const
	{
		if (stack_.size() <= size)
		{
#ifdef _CPU_ENABLE_EXEPTIONS
			throw Stack<T>::OutOfRangeExc("There are not enough elements in stack");
#endif // _CPU_ENABLE_EXEPTIONS

			NDebugger::Error("There are not enough elements in stack, number of elements = " + stack_.size());

			return FALSE;
		}

		return TRUE;
	}

	template<typename T>
	CPU<T> &CPU<T>::operator=(CONST CPU<T> &crCPU)
	{
		if (this != &crCPU)
		{
			reg_   = crCPU.reg_;
			ram_   = crCPU.ram_;
			stack_ = crCPU.stack_;
		}

		return *this;
	}

	template<typename T>
	CPU<T> &CPU<T>::operator=(CPU<T> &&rrCPU)
	{
		assert(this != &rrCPU);

		reg_   = std::move(rrCPU.reg_);
		ram_   = std::move(rrCPU.ram_);
		stack_ = std::move(rrCPU.stack_);

		return *this;
	}

	template<typename T>
	VOID CPU<T>::push(crVal_ val) 
	{ 
		stack_.push(val); 
	}

	template<typename T>
	VOID CPU<T>::push(rrVal_ val) 
	{ 
		stack_.push(val); 
	}

	template<typename T>
	VOID CPU<T>::push(REG reg)
	{
		stack_.push(reg_[reg]);
	}

	template<typename T>
	VOID CPU<T>::pop() 
	{ 
		stack_.pop(); 
	}

	template<typename T>
	BOOL CPU<T>::add()
	{
		if (!CheckStackSize(1)) return FALSE;

		auto a = stack_.top();
		stack_.pop();

		auto b = stack_.top();
		stack_.pop();

		stack_.push(a + b);

		return TRUE;
	}

	template<typename T>
	BOOL CPU<T>::sub()
	{
		if (!CheckStackSize(1)) return FALSE;

		auto a = stack_.top();
		stack_.pop();

		auto b = stack_.top();
		stack_.pop();

		stack_.push(a - b);

		return TRUE;
	}

	template<typename T>
	BOOL CPU<T>::mul()
	{
		if (!CheckStackSize(1)) return FALSE;

		auto a = stack_.top();
		stack_.pop();

		auto b = stack_.top();
		stack_.pop();

		stack_.push(a * b);

		return TRUE;
	}

	template<typename T>
	BOOL CPU<T>::div()
	{
		if (!CheckStackSize(1)) return FALSE;

		auto a = stack_.top();
		stack_.pop();

		auto b = stack_.top();
		stack_.pop();

		assert(b);
		stack_.push(a / b);

		return TRUE;
	}

	template<typename T>
	BOOL CPU<T>::dup()
	{
		if (!CheckStackSize()) return FALSE;

		stack_.push(stack_.top());

		return TRUE;
	}

	template<typename T>
	BOOL CPU<T>::sqrt()
	{
		if (!CheckStackSize()) return FALSE;

		if (std::is_arithmetic<T>::value) assert(!"Type T must be arithmetic\n");

		auto a = stack_.top();
		stack_.pop();

		stack_.push(static_cast<T>(sqrt_(std::is_integral<T>::value ? static_cast<DOUBLE>(a) : a)));

		return TRUE;
	}

	template<typename T>
	BOOL CPU<T>::sin()
	{
		if (!CheckStackSize()) return FALSE;

		if (std::is_arithmetic<T>::value) assert(!"Type T must be arithmetic\n");

		auto a = stack_.top();
		stack_.pop();

		stack_.push(static_cast<T>(sin_(std::is_integral<T>::value ? static_cast<DOUBLE>(a) : a)));

		return TRUE;
	}

	template<typename T>
	BOOL CPU<T>::cos()
	{
		if (!CheckStackSize()) return FALSE;

		if (std::is_arithmetic<T>::value) assert(!"Type T must be arithmetic\n");

		auto a = stack_.top();
		stack_.pop();

		stack_.push(static_cast<T>(cos_(std::is_integral<T>::value ? static_cast<DOUBLE>(a) : a)));

		return TRUE;
	}

	template<typename T>
	std::pair<T, T> CPU<T>::getPair()
	{
		std::pair<T, T> pair;
		if (!CheckStackSize(1)) return pair;

		pair.first = stack_.top();
		stack_.pop();

		pair.second = stack_.top();
		stack_.pop();

		return pair;
	}

	template<typename T>
	VOID CPU<T>::swap(CPU<T> &rCPU) 
	{ 
		stack_.swap(rCPU.stack_); 
		reg_.swap(rCPU.reg_); 
	}

	template<typename T>
	VOID CPU<T>::move(REG src, REG dest) 
	{ 
		reg_[dest] = reg_[src]; 
	}

	template<typename T>
	VOID CPU<T>::move(crVal_ src, REG dest) 
	{ 
		reg_[dest] = src; 
	}

	template<typename T>
	VOID CPU<T>::dump() const
	{
		NDebugger::Info("\n\t\t[CPU DUMP]", NDebugger::TextColors::LightMagenta);

		std::cout << "CPU <" << typeid(T).name() << "> [0x" << this << "]\n\n";

		reg_.dump();
		ram_.dump();
		stack_.dump();

		NDebugger::Info("\t\t[  END   ]\n", NDebugger::TextColors::LightMagenta);
	}
};


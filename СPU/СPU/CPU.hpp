#pragma once

#include <fstream>
#include <string>
#include <cassert>

#include "Register.hpp"
#include "RAM.hpp"
#include "Stack.hpp"
#include "MyMath.hpp"

//cmd -D_SCL_SECURE_NO_WARNINGS

namespace NCpu
{
#pragma region Usings

	using NReg::REG;
	using NReg::Register;

	using NRam::RAM;

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
		using rVal_  = T&;
		using rrVal_ = T&&;
		using crVal_ = CONST T&;

		explicit CPU() noexcept;
		CPU(CONST CPU<T>&) noexcept;
		CPU(CPU<T>&&) noexcept;
		~CPU();

		CPU<T> &operator=(CONST CPU<T>&);
		CPU<T> &operator=(CPU<T>&&);

		VOID push(crVal_);
		VOID push(rrVal_);
		VOID push(REG);
		VOID pop();

		BOOL add();
		BOOL sub();
		BOOL mul();
		BOOL div();
		BOOL dup();

		BOOL sqrt();
		BOOL sin();
		BOOL cos();

		std::pair<T, T> getPair();

		VOID swap(CPU<T>&) noexcept(std::_Is_nothrow_swappable<T>::value);

		inline VOID move(REG, REG);
		inline VOID move(crVal_, REG);

		VOID dump() const;
	};

	template<typename T>
	CPU<T>::CPU() noexcept :
		reg_(),
		ram_(),
		stack_()
	{ }

	template<typename T>
	CPU<T>::CPU(CONST CPU<T> &crCPU) noexcept :
		reg_(crCPU.reg_),
		ram_(crCPU.ram_),
		stack_(crCPU.stack_)
	{ }

	template<typename T>
	CPU<T>::CPU(CPU<T> &&rrCPU) noexcept :
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
	VOID CPU<T>::swap(CPU<T> &rCPU) noexcept(std::_Is_nothrow_swappable<T>::value)
	{ 
		reg_.swap(rCPU.reg_);
		stack_.swap(rCPU.stack_); 
		ram_.swap(rCPU.ram_);
	}

	template<typename T>
	inline VOID CPU<T>::move(REG src, REG dest) 
	{ 
		reg_[dest] = reg_[src]; 
	}

	template<typename T>
	inline VOID CPU<T>::move(crVal_ src, REG dest) 
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
} // namespace NCpu


#pragma once

#include "Register.hpp"
#include "RAM.hpp"
#include "Stack.hpp"
#include "MyMath.hpp"

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

	public:
		typedef       T  &rVal_;
		typedef       T &&rrVal_;
		typedef CONST T  &crVal_;

		explicit CPU()  _NOEXCEPT;
		CPU(CONST CPU&) _NOEXCEPT;
		CPU(CPU&&)      _NOEXCEPT;
		~CPU();

		CPU<T> &operator=(CONST CPU&);
		CPU<T> &operator=(CPU&&);

		VOID push(crVal_);
		VOID push(rrVal_);
		VOID push(REG);
		VOID pop();

		VOID put(crVal_);
		VOID put(rrVal_);
		VOID popm();

		VOID add();
		VOID sub();
		VOID mul();
		VOID div();
		VOID dup();

		VOID sqrt();
		VOID sin();
		VOID cos();

		std::pair<T, T> getPair();

		VOID swap(CPU&) _NOEXCEPTARG(std::_Is_nothrow_swappable<T>);

		VOID move(REG, REG);
		VOID move(crVal_, REG);

		VOID dump() const;
	};

	template<typename T>
	inline CPU<T>::CPU() _NOEXCEPT :
		reg_(),
		ram_(),
		stack_()
	{ }

	template<typename T>
	inline CPU<T>::CPU(CONST CPU &crCPU) _NOEXCEPT :
		reg_(crCPU.reg_),
		ram_(crCPU.ram_),
		stack_(crCPU.stack_)
	{ }

	template<typename T>
	inline CPU<T>::CPU(CPU<T> &&rrCPU) _NOEXCEPT :
		reg_(std::move(rrCPU.reg_)),
		ram_(std::move(rrCPU.ram_)),
		stack_(std::move(rrCPU.stack_))
	{ }

	template<typename T>
	inline CPU<T>::~CPU()
	{ }

	template<typename T>
	inline CPU<T> &CPU<T>::operator=(CONST CPU &crCPU)
	{
		if (this != &crCPU)
		{
			reg_   = crCPU.reg_;
			ram_   = crCPU.ram_;
			stack_ = crCPU.stack_;
		}

		return (*this);
	}

	template<typename T>
	inline CPU<T> &CPU<T>::operator=(CPU<T> &&rrCPU)
	{
		assert(this != &rrCPU);

		reg_   = std::move(rrCPU.reg_);
		ram_   = std::move(rrCPU.ram_);
		stack_ = std::move(rrCPU.stack_);

		return (*this);
	}

	template<typename T>
	inline VOID CPU<T>::push(crVal_ val)
	{ 
		stack_.push(val); 

		reg_[REG::SP] = stack_.top();
	}

	template<typename T>
	inline VOID CPU<T>::push(rrVal_ val) 
	{ 
		stack_.push(val);

		reg_[REG::SP] = stack_.top();
	}

	template<typename T>
	inline VOID CPU<T>::push(REG reg)
	{
		stack_.push(reg_[reg]);

		reg_[REG::SP] = stack_.top();
	}

	template<typename T>
	inline VOID CPU<T>::pop()
	{ 
		stack_.pop(); 

		reg_[REG::SP] = stack_.top();
	}

	template<typename T>
	inline VOID CPU<T>::put(crVal_ val)
	{
		ram_.put(val);
	}

	template<typename T>
	inline VOID CPU<T>::put(rrVal_ val)
	{
		ram_.put(val);
	}

	template<typename T>
	inline VOID CPU<T>::popm()
	{
		ram_.pop();
	}

	template<typename T>
	VOID CPU<T>::add()
	{
		auto a = stack_.top();
		stack_.pop();

		auto b = stack_.top();
		stack_.pop();

		stack_.push(a + b);

		reg_[REG::SP] = stack_.top();
	}

	template<typename T>
	VOID CPU<T>::sub()
	{
		auto a = stack_.top();
		stack_.pop();

		auto b = stack_.top();
		stack_.pop();

		stack_.push(a - b);

		reg_[REG::SP] = stack_.top();
	}

	template<typename T>
	VOID CPU<T>::mul()
	{
		auto a = stack_.top();
		stack_.pop();

		auto b = stack_.top();
		stack_.pop();

		stack_.push(a * b);

		reg_[REG::SP] = stack_.top();
	}

	template<typename T>
	VOID CPU<T>::div()
	{
		auto a = stack_.top();
		stack_.pop();

		auto b = stack_.top();
		stack_.pop();

		if (!b) throw std::logic_error("[CPU::div] \"Division by zero\"\n");
		stack_.push(a / b);

		reg_[REG::SP] = stack_.top();
	}

	template<typename T>
	inline VOID CPU<T>::dup()
	{
		stack_.push(stack_.top());
	}

	template<typename T>
	VOID CPU<T>::sqrt()
	{
		if (std::is_arithmetic<T>::value) assert(!"Type T must be arithmetic\n");

		auto a = stack_.top();
		stack_.pop();

		stack_.push(static_cast<T>(sqrt_(std::is_integral<T>::value ? static_cast<DOUBLE>(a) : a)));

		reg_[REG::SP] = stack_.top();
	}

	template<typename T>
	VOID CPU<T>::sin()
	{
		if (std::is_arithmetic<T>::value) assert(!"Type T must be arithmetic\n");

		auto a = stack_.top();
		stack_.pop();

		stack_.push(static_cast<T>(sin_(std::is_integral<T>::value ? static_cast<DOUBLE>(a) : a)));

		reg_[REG::SP] = stack_.top();
	}

	template<typename T>
	VOID CPU<T>::cos()
	{
		if (std::is_arithmetic<T>::value) assert(!"Type T must be arithmetic\n");

		auto a = stack_.top();
		stack_.pop();

		stack_.push(static_cast<T>(cos_(std::is_integral<T>::value ? static_cast<DOUBLE>(a) : a)));

		reg_[REG::SP] = stack_.top();
	}

	template<typename T>
	std::pair<T, T> CPU<T>::getPair()
	{
		std::pair<T, T> pair;

		pair.first = stack_.top();
		stack_.pop();

		pair.second = stack_.top();
		stack_.pop();

		reg_[REG::SP] = stack_.top();

		return pair;
	}

	template<typename T>
	inline VOID CPU<T>::swap(CPU &rCPU) _NOEXCEPTARG(std::_Is_nothrow_swappable<T>)
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
		NDebugger::Info("\n\t\t[CPU DUMP]", NDebugger::TextColor::LightMagenta);

		std::cout << "CPU <" << typeid(T).name() << "> [0x" << this << "]\n\n";

		reg_.dump();
		ram_.dump();
		stack_.dump();

		NDebugger::Info("\t\t[  END   ]\n", NDebugger::TextColor::LightMagenta);
	}
} // namespace NCpu


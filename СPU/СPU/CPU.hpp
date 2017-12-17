#pragma once

#include <fstream>
#include <string>
#include <cassert>
#include <type_traits>
#include <array>

#include "Stack.hpp"
#include "MyMath.hpp"

//cmd -D_SCL_SECURE_NO_WARNINGS

enum Registers_ : SIZE_T
{
	AX,
	BX,
	CX,
	DX,
	EX,

	SP,

	NUM
};
typedef Registers_ REG;

template<typename T = INT>
struct Register final
{
	std::array<T, REG::NUM> regs;

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

	T &operator[](REG reg) { assert(reg != REG::NUM); return regs[reg]; }

	VOID swap(Register<T> &rReg) { reg.swap(rReg.reg); }

	VOID dump() const
	{
		Debugger::Info("[REGISTER DUMP]", Debugger::TextColors::Green);

		for (SIZE_T i = 0; i < REG::NUM - 1; ++i) std::cout << static_cast<CHAR>('A' + i) << "X: " << regs[i] << std::endl;

		std::cout << "SP: " << regs[REG::SP] << std::endl << std::endl;
	}
};

//==================================================================================================================

template<typename T = INT>
class CPU final
{	
	Register<T> reg_;
	Stack<T>    stack_;

	BOOL CheckStackSize(SIZE_T = 0) const;

public:
	typedef       T  &rVal_;
	typedef       T &&rrVal_;
	typedef typename CONST T  &crVal_;

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
	{ *this = std::move(rrProc); } // TODO: do not be lazyboy

	~CPU() { }

	CPU<T> &operator=(CONST CPU<T>&);
	CPU<T> &operator=(CPU<T>&&);
	
	SIZE_T getStackSize() const { return stack_.size(); }

	VOID   push(crVal_ val) { stack_.push(val); }
	VOID   push(rrVal_ val) { stack_.push(val); }
	VOID   pop()            { stack_.pop(); } 
	
	crVal_ top() const { return stack_.top(); }
	
	BOOL add();
	BOOL sub();
	BOOL mul();
	BOOL div();
	BOOL dup();

	BOOL sqrt();
	BOOL sin();
	BOOL cos();

	std::pair<T, T> getPair();

	VOID swap(CPU<T> &rCPU) { stack_.swap(rCPU.stack_); reg_.swap(rCPU.reg_); }

	VOID move(REG src, REG dest)            { reg_[dest] = reg_[src]; }
	VOID move(CONST T &src, REG dest)       { reg_[dest] = src; }

	//VOID move(REG src, T &dest)      const { dest = reg_[src]; }
	//VOID move(CONST T &src, T &dest) const { dest = src; }

	VOID dump() const 
	{ 
		Debugger::Info("\n\t\t[CPU DUMP]", Debugger::TextColors::LightMagenta);

		reg_.dump(); 
		stack_.dump(); 

		Debugger::Info("\t\t[  END   ]\n", Debugger::TextColors::LightMagenta);
	}
};

template<typename T>
BOOL CPU<T>::CheckStackSize(SIZE_T size /* = 0 */) const
{
	if (stack_.size() <= size)
	{
#ifdef _CPU_ENABLE_EXEPTIONS
		throw Stack<T>::OutOfRangeExc("There are not enough elements in stack");
#endif // _CPU_ENABLE_EXEPTIONS

		Debugger::Error("There are not enough elements in stack, number of elements = " + stack_.size());

		return FALSE;
	}

	return TRUE;
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
	if(!CheckStackSize(1)) return FALSE;

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

	if(std::is_arithmetic<T>::value) assert(!"Type T must be arithmetic\n");

	auto a = stack_.top();
	stack_.pop();

	stack_.push(static_cast<T>(sqrt_(std::is_integral<T>::value? static_cast<DOUBLE>(a) : a)));

	return TRUE;
}

template<typename T>
BOOL CPU<T>::sin()
{
	if (!CheckStackSize()) return FALSE;

	if(std::is_arithmetic<T>::value) assert(!"Type T must be arithmetic\n");

	auto a = stack_.top();
	stack_.pop();

	stack_.push(static_cast<T>(sin_(std::is_integral<T>::value? static_cast<DOUBLE>(a) : a)));

	return TRUE;
}

template<typename T>
BOOL CPU<T>::cos()
{
	if (!CheckStackSize()) return FALSE;

	if(std::is_arithmetic<T>::value) assert(!"Type T must be arithmetic\n");

	auto a = stack_.top();
	stack_.pop();

	stack_.push(static_cast<T>(cos_(std::is_integral<T>::value? static_cast<DOUBLE>(a) : a)));

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



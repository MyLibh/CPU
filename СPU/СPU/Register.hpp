#pragma once

#include <array>   // std::array
#include <cassert> // assert

#include "MyTypedefs.hpp"
#include "Debugger.hpp"

namespace NReg
{
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
		typedef       T &rVal_;
		typedef CONST T &crVal_;

		std::array<T, REG::NUM> regs;

		explicit Register()       noexcept;
		Register(CONST Register&) noexcept;
		Register(Register&&)      noexcept;
		~Register();

		Register &operator=(CONST Register&);
		Register<T> &operator=(Register&&);

		rVal_  operator[](REG);
		crVal_ operator[](REG) const;

		VOID swap(Register&) noexcept(std::_Is_nothrow_swappable<T>::value);

		VOID dump() const;
	};

	template<typename T>
	inline Register<T>::Register() noexcept :
		regs()
	{ }

	template<typename T>
	inline Register<T>::Register(CONST Register &crReg) noexcept :
		regs(crReg.regs)
	{ }

	template<typename T>
	inline Register<T>::Register(Register &&rrReg) noexcept :
		regs(std::move(rrReg.regs))
	{
		// rrReg.regs.fill(NULL); // QUEST: should be or not(then no noexcept)
	}

	template<typename T>
	inline Register<T>::~Register()
	{ }

	template<typename T>
	inline Register<T> &Register<T>::operator=(CONST Register &crReg)
	{
		if (this != &crReg) regs = crReg.regs;

		return (*this);
	}

	template<typename T>
	inline Register<T> &Register<T>::operator=(Register &&rrReg)
	{
		assert(this != &rrReg);

		regs = std::move(rrReg.regs);

		// rrReg.regs.fill(NULL); // QUEST: should be or not(then no noexcept)

		return (*this);
	}

	template<typename T>
	inline typename Register<T>::rVal_ Register<T>::operator[](REG reg) 
	{
		if (reg == REG::NUM) throw std::out_of_range("[Register::operator[]] \"Register out of range\"\n");

		return regs[reg];
	}

	template<typename T>
	inline typename Register<T>::crVal_ Register<T>::operator[](REG reg) const 
	{ 
		if (reg == REG::NUM) throw std::out_of_range("[Register::operator[]] \"Register out of range\"\n");
		
		return regs[reg]; 
	}

	template<typename T>
	inline VOID Register<T>::swap(Register &rReg) noexcept(std::_Is_nothrow_swappable<T>::value)
	{ 
		reg.swap(rReg.reg); 
	}

	template<typename T>
	VOID Register<T>::dump() const
	{
		NDebugger::Info("\t[REGISTER DUMP]", NDebugger::TextColors::Green);

		std::cout << "Register <" << typeid(T).name() << "> [0x" << this << "]\n{\n";

		for (SIZE_T i = 0; i < REG::NUM - 1; ++i) std::cout << "\t[" << static_cast<CHAR>('A' + i) << "X] = " << regs[i] << std::endl;

		std::cout << "\t[SP] = " << regs[REG::SP] << "\n}\n";

		NDebugger::Info("\t[     END     ]\n", NDebugger::TextColors::Green);
	}
} // namespace NReg 
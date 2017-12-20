#pragma once

#include <array>
#include <type_traits>

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
		std::array<T, REG::NUM> regs;

		explicit Register() noexcept;
		Register(CONST Register<T>&) noexcept;
		Register(Register<T>&&) noexcept;
		~Register();

		Register<T> &operator=(CONST Register<T>&);
		Register<T> &operator=(Register<T>&&);

		T &operator[](REG);
		CONST T &operator[](REG) const;

		inline VOID swap(Register<T>&) noexcept(std::_Is_nothrow_swappable<T>::value);

		VOID dump() const;
	};

	template<typename T>
	Register<T>::Register() noexcept :
		regs()
	{ }

	template<typename T>
	Register<T>::Register(CONST Register<T> &crReg) noexcept :
		regs(crReg.regs)
	{ }

	template<typename T>
	Register<T>::Register(Register<T> &&rrReg) noexcept :
		regs(std::move(rrReg.regs))
	{ }

	template<typename T>
	Register<T>::~Register()
	{ }

	template<typename T>
	Register<T> &Register<T>::operator=(CONST Register<T> &crReg)
	{
		if (this != &crReg) regs = crReg.regs;

		return *this;
	}

	template<typename T>
	Register<T> &Register<T>::operator=(Register<T> &&rrReg)
	{
		assert(this != &rrReg);

		regs = std::move(rrReg.regs);

		return *this;
	}

	template<typename T>
	T &Register<T>::operator[](REG reg) 
	{
		assert(reg != REG::NUM);

		return regs[reg];
	}

	template<typename T>
	CONST T &Register<T>::operator[](REG reg) const 
	{ 
		assert(reg != REG::NUM); 
		
		return regs[reg]; 
	}

	template<typename T>
	inline VOID Register<T>::swap(Register<T> &rReg) noexcept(std::_Is_nothrow_swappable<T>::value)
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
}  // namespace NReg
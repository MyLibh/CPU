#pragma once

#include <array>   // std::array
#include <cassert> // assert

#include "Debugger.hpp"
#include "Guard.hpp"
#include "Logger.hpp"

extern Logger gLogger;

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
	class Register final
	{
	public:
		typedef       T &rVal_;
		typedef CONST T &crVal_;

		explicit Register()       _NOEXCEPT;
		Register(CONST Register&) _NOEXCEPT;
		Register(Register&&)      _NOEXCEPT;
		~Register();

		Register &operator=(CONST Register&) _NOEXCEPT;
		Register<T> &operator=(Register&&)   _NOEXCEPT;

		rVal_  operator[](REG);
		crVal_ operator[](REG) const;

		HASH_GUARD(inline VOID rehash() { hash_ = makeHash(); })

		VOID swap(Register&) _NOEXCEPTARG(std::_Is_nothrow_swappable<T>);

		BOOL ok() const _NOEXCEPT;
		VOID dump(std::ostream& = std::cout) const;

	private:
		CANARY_GUARD(CONST std::string CANARY_VALUE = NHash::Hash("Register").getHash();)

		CANARY_GUARD(std::string canaryStart_;)
		HASH_GUARD(std::string hash_;)

		std::array<T, REG::NUM> regs_;

		CANARY_GUARD(std::string canaryFinish_;)

		HASH_GUARD
		(
			std::string makeHash() const
			{
				std::string tmp;
				for (SIZE_T i = 0; i < REG::NUM; ++i) tmp += std::to_string(regs_[i]);

				return NHash::Hash(tmp).getHash();
			}
		)
	};

	//====================================================================================================================================

	template<typename T>
	Logger& operator<<(Logger&, CONST Register<T>&);

	//====================================================================================================================================

	template<typename T>
	inline Register<T>::Register() _NOEXCEPT :
		CANARY_GUARD(canaryStart_(CANARY_VALUE),)
		HASH_GUARD(hash_(),)

		regs_()

		CANARY_GUARD(, canaryFinish_(CANARY_VALUE))
	{ 
		HASH_GUARD(hash_ = makeHash();)

		LOG_CONSTRUCTING()

		GUARD_CHECK()
	}

	template<typename T>
	inline Register<T>::Register(CONST Register &crReg) _NOEXCEPT :
		CANARY_GUARD(canaryStart_(CANARY_VALUE), )
		HASH_GUARD(hash_(crReg.hash_),)

		regs_(crReg.regs_)

		CANARY_GUARD(, canaryFinish_(CANARY_VALUE))
	{ 
		LOG_CONSTRUCTING()

		GUARD_CHECK()
	}

	template<typename T>
	inline Register<T>::Register(Register &&rrReg) _NOEXCEPT :
		CANARY_GUARD(canaryStart_(CANARY_VALUE),)
		HASH_GUARD(hash_(std::move(rrReg.hash_)),)

		regs_(std::move(rrReg.regs_))

		CANARY_GUARD(, canaryFinish_(CANARY_VALUE))
	{
		rrReg.regs_.fill(NULL);

		HASH_GUARD(rrReg.hash_.clear();)

		LOG_CONSTRUCTING()

		GUARD_CHECK()
	}

	template<typename T>
	inline Register<T>::~Register()
	{
		LOG_DESTRUCTING()
		GUARD_CHECK()
	}

	template<typename T>
	inline Register<T> &Register<T>::operator=(CONST Register &crReg) _NOEXCEPT
	{
		GUARD_CHECK()

		if (this != &crReg)
		{
			regs_ = crReg.regs_;

			HASH_GUARD(rehash();)
		}

		GUARD_CHECK()

		return (*this);
	}

	template<typename T>
	inline Register<T> &Register<T>::operator=(Register &&rrReg) _NOEXCEPT
	{
		GUARD_CHECK()

		assert(this != &rrReg);

		regs_ = std::move(rrReg.regs_);
		HASH_GUARD(hash_ = std::move(rrReg.hash_);)

		rrReg.regs_.fill(NULL);
		HASH_GUARD(rrReg.hash_.clear();)

		GUARD_CHECK()

		return (*this);
	}

	template<typename T>
	inline typename Register<T>::rVal_ Register<T>::operator[](REG reg) 
	{
		GUARD_CHECK()

		if (reg == REG::NUM) throw std::out_of_range("[Register::operator[]] \"Register out of range\"\n");

		GUARD_CHECK()

		return regs_[reg];
	}

	template<typename T>
	inline typename Register<T>::crVal_ Register<T>::operator[](REG reg) const 
	{ 
		GUARD_CHECK()

		if (reg == REG::NUM) throw std::out_of_range("[Register::operator[]] \"Register out of range\"\n");
		
		GUARD_CHECK()

		return regs_[reg]; 
	}

	template<typename T>
	inline VOID Register<T>::swap(Register &rReg) _NOEXCEPTARG(std::_Is_nothrow_swappable<T>)
	{ 
		GUARD_CHECK()

		using std::swap;

		swap(reg, rReg.reg); 
		HASH_GUARD(swap(reg, rrReg.hash_);)

		GUARD_CHECK()
	}

	template<typename T>
	inline BOOL Register<T>::ok() const _NOEXCEPT
	{
		return (this &&
				CANARY_GUARD(canaryStart_ == CANARY_VALUE && canaryFinish_ == CANARY_VALUE && )
				HASH_GUARD(hash_ == makeHash() && ) 
				regs_.size());
	}

	template<typename T>
	VOID Register<T>::dump(std::ostream &rOstr /* = std::cout */) const
	{
		NDebugger::Info("\t[REGISTER DUMP]", NDebugger::TextColor::Green, TRUE, rOstr);

		rOstr << "Register <" << typeid(T).name() << "> [0x" << this << "]\n{\n";

		for (SIZE_T i = 0; i < REG::NUM - 1; ++i) rOstr << "\t[" << static_cast<CHAR>('A' + i) << "X] = " << regs_[i] << std::endl;

		rOstr << "\t[SP] = " << regs_[REG::SP] << "\n\n";

		CANARY_GUARD
		(
			rOstr << "\tCANARY_VALUE  = " << CANARY_VALUE << std::endl;

			rOstr << "\tCANARY_START  = " << canaryStart_;
			if (canaryStart_ == CANARY_VALUE) NDebugger::Info(" TRUE",  NDebugger::TextColor::Green, TRUE, rOstr);
			else                              NDebugger::Info(" FALSE", NDebugger::TextColor::Red,   TRUE, rOstr);

			rOstr << "\tCANARY_FINISH = " << canaryFinish_;
			if (canaryFinish_ == CANARY_VALUE) NDebugger::Info(" TRUE",  NDebugger::TextColor::Green, TRUE, rOstr);
			else                               NDebugger::Info(" FALSE", NDebugger::TextColor::Red,   TRUE, rOstr);
		)

		HASH_GUARD
		(
			rOstr << "\n\tHASH = " << hash_;
			if (hash_ == makeHash()) NDebugger::Info(" TRUE",  NDebugger::TextColor::Green, TRUE, rOstr);
			else                     NDebugger::Info(" FALSE", NDebugger::TextColor::Red,   TRUE, rOstr);
		)

		rOstr << "}\n";

		NDebugger::Info("\t[     END     ]\n", NDebugger::TextColor::Green, TRUE, rOstr);
	}

	//====================================================================================================================================

	template<typename T>
	Logger& operator<<(Logger &rLogger, CONST Register<T> &crRegister)
	{
		std::string func("Register<");
		func += typeid(T).name();
		func += ">";

		rLogger.stdPack(func);

		crRegister.dump(rLogger.getOfstream());

		return rLogger;
	}
} // namespace NReg 
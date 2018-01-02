#pragma once

#include "Debugger.hpp"
#include "Storage.hpp"
#include "Logger.hpp"

namespace NRegister
{
	enum Registers_ : SIZE_T
	{
		AX,
		BX,
		CX,
		DX,
		EX,

		SP, // Last element in stack

		NUM
	};
	typedef Registers_ REG;

	template<typename T = INT>
	class Register final : public Storage<T, REG::NUM>
	{
	public:
		explicit Register()       _NOEXCEPT;
		Register(CONST Register&) _NOEXCEPT;
		Register(Register&&)      _NOEXCEPT;
		~Register();

		virtual VOID dump(std::ostream& = std::cout) const override;
	};

	//====================================================================================================================================

	template<typename T>
	Logger& operator<<(Logger&, CONST Register<T>&);

	//====================================================================================================================================

	template<typename T>
	inline Register<T>::Register() _NOEXCEPT :
		Storage()
	{
		LOG_CONSTRUCTING()
	}

	template<typename T>
	inline Register<T>::Register(CONST Register &crRegister) _NOEXCEPT :
		Storage(crRegister)
	{
		LOG_CONSTRUCTING()
	}

	template<typename T>
	inline Register<T>::Register(Register &&rrRegister) _NOEXCEPT :
		Storage(std::move(rrRegister))
	{
		LOG_CONSTRUCTING()
	}

	template<typename T>
	inline Register<T>::~Register()
	{
		LOG_DESTRUCTING()
	}

	template<typename T>
	VOID Register<T>::dump(std::ostream &rOstr /* = std::cout */) const 
	{
		NDebugger::Info("\t[REGISTER DUMP]", NDebugger::TextColor::Green, TRUE, rOstr);

		rOstr << "Register <" << typeid(T).name() << "> [0x" << this << "]\n{\n";

		for (SIZE_T i = 0; i < REG::NUM - 1; ++i) rOstr << "\t[" << static_cast<CHAR>('A' + i) << "X] = " << buf_[i] << std::endl;

		rOstr << "\t[SP] = " << buf_[REG::SP] << "\n\n";

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
			rOstr << "\n\tHASH = " << getHash();
			if (getHash() == makeHash()) NDebugger::Info(" TRUE",  NDebugger::TextColor::Green, TRUE, rOstr);
			else                         NDebugger::Info(" FALSE", NDebugger::TextColor::Red,   TRUE, rOstr);
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
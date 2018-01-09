#pragma once

#include "Debugger.hpp"
#include "Storage.hpp"
#include "Logger.hpp"

namespace NRegister
{
	enum class Registers : size_t
	{
		AX,
		BX,
		CX,
		DX,
		EX,

		SP, // Last element in stack

		NUM
	};
	typedef Registers REG;

	template<typename T = int>
	class Register final : public Storage<T, static_cast<size_t>(REG::NUM)>
	{
	public:
		explicit Register()       noexcept;
		Register(const Register&) noexcept;
		Register(Register&&)      noexcept;
		~Register();

		virtual void dump(std::ostream& = std::cout) const override;
	};

	//====================================================================================================================================

	template<typename T>
	Logger& operator<<(Logger&, const Register<T>&);

	inline std::string_view GetReg(REG);

	//====================================================================================================================================

	template<typename T>
	inline Register<T>::Register() noexcept :
		Storage()
	{
		LOG_CONSTRUCTING()
	}

	template<typename T>
	inline Register<T>::Register(const Register &crRegister) noexcept :
		Storage(crRegister)
	{
		LOG_CONSTRUCTING()
	}

	template<typename T>
	inline Register<T>::Register(Register &&rrRegister) noexcept :
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
	void Register<T>::dump(std::ostream &rOstr /* = std::cout */) const 
	{
		NDebugger::Info("\t[REGISTER DUMP]", NDebugger::TextColor::Green, true, rOstr);

		rOstr << "Register <" << typeid(T).name() << "> [0x" << this << "]\n{\n";

		for (size_t i = 0; i < static_cast<size_t>(REG::NUM) - 1; ++i) rOstr << "\t[" << static_cast<char>('A' + i) << "X] = " << buf_[i] << std::endl;

		rOstr << "\t[SP] = " << buf_[static_cast<size_t>(REG::SP)] << "\n\n";

		CANARY_GUARD
		(
			rOstr << "\tCANARY_VALUE  = " << CANARY_VALUE << std::endl;

			rOstr << "\tCANARY_START  = " << canaryStart_;
			if (canaryStart_ == CANARY_VALUE) NDebugger::Info(" TRUE",  NDebugger::TextColor::Green, true, rOstr);
			else                              NDebugger::Info(" FALSE", NDebugger::TextColor::Red,   true, rOstr);

			rOstr << "\tCANARY_FINISH = " << canaryFinish_;
			if (canaryFinish_ == CANARY_VALUE) NDebugger::Info(" TRUE",  NDebugger::TextColor::Green, true, rOstr);
			else                               NDebugger::Info(" FALSE", NDebugger::TextColor::Red,   true, rOstr);
		)

		HASH_GUARD
		(
			rOstr << "\n\tHASH = " << getHash().data();
			if (getHash() == makeHash()) NDebugger::Info(" TRUE",  NDebugger::TextColor::Green, true, rOstr);
			else                         NDebugger::Info(" FALSE", NDebugger::TextColor::Red,   true, rOstr);
		)

		rOstr << "}\n";

		NDebugger::Info("\t[     END     ]\n", NDebugger::TextColor::Green, true, rOstr);
	}

	//====================================================================================================================================

	template<typename T>
	Logger& operator<<(Logger &rLogger, const Register<T> &crRegister)
	{
		std::string func("Register<");
		func += typeid(T).name();
		func += ">";

		rLogger.stdPack(func);

		crRegister.dump(rLogger.getOfstream());

		return rLogger;
	}

	inline std::string_view GetReg(REG reg)
	{
		if      (reg == REG::AX) return std::string_view("AX");
		else if (reg == REG::BX) return std::string_view("BX");
		else if (reg == REG::CX) return std::string_view("CX");
		else if (reg == REG::DX) return std::string_view("DX");
		else if (reg == REG::EX) return std::string_view("EX");
		else if (reg == REG::SP) return std::string_view("SP");
		else                     return std::string_view("null");
	}
} // namespace NReg 
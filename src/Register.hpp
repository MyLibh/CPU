#pragma once

#include "Debugger.hpp"
#include "Storage.hpp"
#include "Logger.hpp"

namespace NRegister
{

#pragma region ENUMS

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

#pragma endregion

#pragma region TYPEDEFS

	typedef Registers REG;

#pragma endregion

#pragma region CLASSES

	template<typename T = int>
	class Register final : public Storage<T, static_cast<size_t>(REG::NUM)>
	{
	public:
		explicit Register()       noexcept;
		Register(const Register&) noexcept;
		Register(Register&&)      noexcept;
		~Register();

//====================================================================================================================================
//!
//! \brief	 Dumps register to the stream
//!
//! \param   rOstr  Stream to output
//!
//! \throw   smth
//!
//====================================================================================================================================

		template<typename Char, typename Traits = std::char_traits<Char>>
		void dump(std::basic_ostream<Char, Traits>& rOstr) const;
	};

#pragma endregion

#pragma region FUNCTION_DECLARATION

	template<typename T>
	Logger& operator<<(Logger&, const Register<T>&);

	inline std::string_view GetReg(REG);

	REG MakeReg(std::string_view);

	inline bool IsReg(std::string_view);

#pragma endregion

#pragma region METHOD_DEFINITION

	template<typename T>
	inline Register<T>::Register() noexcept :
		Storage<T, static_cast<size_t>(REG::NUM)>()
	{
		LOG_CONSTRUCTING()
	}

	template<typename T>
	inline Register<T>::Register(const Register &crRegister) noexcept :
		Storage<T, static_cast<size_t>(REG::NUM)>(crRegister)
	{
		LOG_CONSTRUCTING()
	}

	template<typename T>
	inline Register<T>::Register(Register &&rrRegister) noexcept :
		Storage<T, static_cast<size_t>(REG::NUM)>(std::move(rrRegister))
	{
		LOG_CONSTRUCTING()
	}

	template<typename T>
	inline Register<T>::~Register()
	{
		LOG_DESTRUCTING()
	}

	template<typename T>
	template<typename Char, typename Traits>
	void Register<T>::dump(std::basic_ostream<Char, Traits> &rOstr) const
	{
		NDebugger::Text(std::string_view("\t[REGISTER DUMP]"), rOstr, NDebugger::Colors::Green);

		rOstr << "Register <" << typeid(T).name() << "> [0x" << this << "]\n{\n";

		for (size_t i = 0; i < static_cast<size_t>(REG::NUM) - 1; ++i) 
			rOstr << "\t[" << static_cast<char>('A' + i) << "X] = " << this->buf_[i] << std::endl;

		rOstr << "\t[SP] = " << this->buf_[static_cast<size_t>(REG::SP)] << "\n\n";

		CANARY_GUARD
		(
			rOstr << "\tCANARY_VALUE  = " << CANARY_VALUE << std::endl;

			rOstr << "\tCANARY_START  = " << canaryStart_;
			if (canaryStart_ == CANARY_VALUE) NDebugger::Text(std::string_view(" TRUE "), rOstr, NDebugger::Colors::Green);
			else                              NDebugger::Text(std::string_view(" FALSE"), rOstr, NDebugger::Colors::Red);

			rOstr << "\tCANARY_FINISH = " << canaryFinish_;
			if (canaryFinish_ == CANARY_VALUE) NDebugger::Text(std::string_view(" TRUE "), rOstr, NDebugger::Colors::Green);
			else                               NDebugger::Text(std::string_view(" FALSE"), rOstr, NDebugger::Colors::Red);
		)

		HASH_GUARD
		(
			rOstr << "\n\tHASH = " << getHash().data();
			if (getHash() == makeHash()) NDebugger::Text(std::string_view(" TRUE "), rOstr, NDebugger::Colors::Green);
			else                         NDebugger::Text(std::string_view(" FALSE"), rOstr, NDebugger::Colors::Red);
		)

		rOstr << "}\n";

		NDebugger::Text(std::string_view("\t[     END     ]\n"), rOstr, NDebugger::Colors::Green);
	}

#pragma endregion

#pragma region FUNCTION_DEFINITION

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

	REG MakeReg(std::string_view str)
	{
		if (str == "ax" || str == "[ax]") return REG::AX;
		else if (str == "bx" || str == "[bx]") return REG::BX;
		else if (str == "cx" || str == "[cx]") return REG::CX;
		else if (str == "dx" || str == "[dx]") return REG::DX;
		else if (str == "ex" || str == "[ex]") return REG::EX;
		else if (str == "sp" || str == "[sp]") return REG::SP;

		else                                   return REG::NUM;
	}

	inline bool IsReg(std::string_view val)
	{
		return (MakeReg(val) != REG::NUM);
	}

#pragma endregion

} // namespace NRegister 
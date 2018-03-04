#pragma once

#include <string_view> // std::string_view
#include <array>       // std::array
#include <functional>  // std::function
#include <sstream>     // std::stringstream

#include "CPU.hpp"

namespace NCpu::Commands
{

//====================================================================================================================================
//===============================================================ENUMS================================================================
//====================================================================================================================================

	enum class Commands : unsigned
	{
		undefined = 4294967295,

		push = 0,
		pop,

		add,
		sub,
		mul,
		div,
		sqrt,
		dup,
		sin,
		cos,

		dump,

		cmp,
		jump,

		je,  // jump if equal
		jne, // jump if not equal
		ja,  // jump if above
		jae, // jump if above or equal
		jb,  // jump if below
		jbe, // jump if below or equal

		move,

		call,
		ret,

		end,

		NUM
	};

//====================================================================================================================================
//=============================================================CONSTANTS==============================================================
//====================================================================================================================================

	constexpr size_t MAX_ARGS = (1 << 2) - 1;

//====================================================================================================================================
//=============================================================TYPEDEFS===============================================================
//====================================================================================================================================

	typedef std::array<std::string, MAX_ARGS> args_t;

//====================================================================================================================================
//==============================================================CLASSES===============================================================
//====================================================================================================================================

	template<typename T>
	class Command
	{
	public:
		std::string_view                                  name;
		Commands                                          number;
		char                                              numOfArgs;
		std::function<short(NCpu::CPU<T>&, const args_t&)> func;

		explicit Command() :
			name("null"),
			number(Commands::undefined),
			numOfArgs('0'),
			func(nullptr)
		{ }

		Command(std::string_view str, Commands com, char num, std::function<short(NCpu::CPU<T>&, const args_t&)> cpuFunc) :
			name(str),
			number(com),
			numOfArgs(num),
			func(cpuFunc)
		{ }

		Command(const Command &crCommand) :
			name(crCommand.name),
			number(crCommand.number),
			numOfArgs(crCommand.numOfArgs),
			func(crCommand.func)
		{ }

		Command &operator=(const Command&) = default;
		Command &operator=(Command&&) = default;
	};

//====================================================================================================================================
//========================================================FUNCTION_DECLARATION========================================================
//====================================================================================================================================

#pragma region FUNCTION_DECLARATION

	template<typename T>
	T GetValue(std::string_view);

	template<typename T>
	short cpu_push(CPU<T>&, const args_t&);

	template<typename T>
	short cpu_pop(CPU<T>&, const args_t&);

	template<typename T>
	short cpu_add(CPU<T>&, const args_t&);

	template<typename T>
	short cpu_sub(CPU<T>&, const args_t&);

	template<typename T>
	short cpu_mul(CPU<T>&, const args_t&);

	template<typename T>
	short cpu_div(CPU<T>&, const args_t&);

	template<typename T>
	short cpu_sqrt(CPU<T>&, const args_t&);

	template<typename T>
	short cpu_dup(CPU<T>&, const args_t&);

	template<typename T>
	short cpu_sin(CPU<T>&, const args_t&);

	template<typename T>
	short cpu_cos(CPU<T>&, const args_t&);

	template<typename T>
	short cpu_dump(CPU<T>&, const args_t&);

	template<typename T>
	short cpu_cmp(CPU<T>&, const args_t&);

	template<typename T>
	short cpu_jump(CPU<T>&, const args_t&);

	template<typename T>
	short cpu_je(CPU<T>&, const args_t&);

	template<typename T>
	short cpu_jne(CPU<T>&, const args_t&);

	template<typename T>
	short cpu_ja(CPU<T>&, const args_t&);

	template<typename T>
	short cpu_jae(CPU<T>&, const args_t&);

	template<typename T>
	short cpu_jb(CPU<T>&, const args_t&);

	template<typename T>
	short cpu_jbe(CPU<T>&, const args_t&);

	template<typename T>
	short cpu_move(CPU<T>&, const args_t&);

	template<typename T>
	short cpu_call(CPU<T>&, const args_t&);

	template<typename T>
	short cpu_ret(CPU<T>&, const args_t&);

#pragma endregion 

//====================================================================================================================================
//==============================================================CLASSES===============================================================
//====================================================================================================================================

	template<typename T>
	ref struct CPU_COMMANDS final
	{
		static const std::array<Command<T>, static_cast<size_t>(Commands::NUM)> buf;

		static auto begin() noexcept -> decltype(buf.begin())
		{
			return buf.begin();
		}

		static auto end() noexcept -> decltype(buf.end())
		{
			return buf.end();
		}

		static auto cbegin() noexcept -> decltype(buf.cbegin())
		{
			return buf.cbegin();
		}

		static auto cend() noexcept -> decltype(buf.cend())
		{
			return buf.cend();
		}
	};

//====================================================================================================================================
//==========================================================STATIC_VARIABLES==========================================================
//====================================================================================================================================

	template<typename T>
	const std::array<Command<T>, static_cast<size_t>(Commands::NUM)> CPU_COMMANDS<T>::buf =
	{
		Command<T>("push", Commands::push, '1', cpu_push<T>),
		Command<T>("pop",  Commands::pop,  '1', cpu_pop<T>),
		Command<T>("add",  Commands::add,  '0', cpu_add<T>),
		Command<T>("sub",  Commands::sub,  '0', cpu_sub<T>),
		Command<T>("mul",  Commands::mul,  '0', cpu_mul<T>),
		Command<T>("div",  Commands::div,  '0', cpu_div<T>),
		Command<T>("sqrt", Commands::sqrt, '0', cpu_sqrt<T>),
		Command<T>("dup",  Commands::dup,  '0', cpu_dup<T>),
		Command<T>("sin",  Commands::sin,  '0', cpu_sin<T>),
		Command<T>("cos",  Commands::cos,  '0', cpu_cos<T>),
		Command<T>("dump", Commands::dump, '0', cpu_dump<T>),
		Command<T>("cmp",  Commands::cmp,  '2', cpu_cmp<T>),
		Command<T>("jump", Commands::jump, '1', cpu_jump<T>),
		Command<T>("je",   Commands::je,   '1', cpu_je<T>),
		Command<T>("jne",  Commands::jne,  '1', cpu_jne<T>),
		Command<T>("ja",   Commands::ja,   '1', cpu_ja<T>),
		Command<T>("jae",  Commands::jae,  '1', cpu_jae<T>),
		Command<T>("jb",   Commands::jb,   '1', cpu_jb<T>),
		Command<T>("jbe",  Commands::jbe,  '1', cpu_jbe<T>),
		Command<T>("move", Commands::move, '2', cpu_move<T>),
		Command<T>("call", Commands::call, '1', cpu_call<T>),
		Command<T>("ret",  Commands::ret,  '0', cpu_ret<T>),
		Command<T>("end",  Commands::end,  '0', nullptr)
	};

//====================================================================================================================================
//========================================================FUNCTION_DEFINITION=========================================================
//====================================================================================================================================

#pragma region FUNCTION_DEFINITION

	template<typename T>
	T GetValue(std::string_view str)
	{
		if (str.front() == '[')
			str.remove_prefix(1);

		std::stringstream sstr(str.data());
		T val = T();
		sstr >> val;

		return val;
	}

	template<typename T>
	short cpu_push(CPU<T> &rCPU, const args_t &crArgs)
	{
		auto val = GetValue<T>(crArgs[0]);
		REG  reg = NRegister::MakeReg(crArgs[0]);

		if (crArgs[0][0] == '[' && reg != REG::NUM)
			rCPU.push(reg, CPU<>::MemoryStorage::RAM);
		else if (crArgs[0][0] == '[' && reg == REG::NUM)
			rCPU.push(val, CPU<>::MemoryStorage::RAM);
		else if (reg != REG::NUM)
			rCPU.push(reg, CPU<>::MemoryStorage::STACK);
		else
			rCPU.push(val, CPU<>::MemoryStorage::STACK);

		return 0;
	}

	template<typename T>
	short cpu_pop(CPU<T> &rCPU, const args_t &crArgs)
	{
		if (crArgs[0][0] == '[') rCPU.pop(CPU<>::MemoryStorage::RAM);
		else                     rCPU.pop(CPU<>::MemoryStorage::STACK);

		return 0;
	}

	template<typename T>
	short cpu_add(CPU<T> &rCPU, const args_t&)
	{
		rCPU.add();

		return 0;
	}

	template<typename T>
	short cpu_sub(CPU<T> &rCPU, const args_t&)
	{
		rCPU.sub();

		return 0;
	}

	template<typename T>
	short cpu_mul(CPU<T> &rCPU, const args_t&)
	{
		rCPU.mul();

		return 0;
	}

	template<typename T>
	short cpu_div(CPU<T> &rCPU, const args_t&)
	{
		rCPU.div();

		return 0;
	}

	template<typename T>
	short cpu_sqrt(CPU<T> &rCPU, const args_t&)
	{
		rCPU.sqrt();

		return 0;
	}

	template<typename T>
	short cpu_dup(CPU<T> &rCPU, const args_t&)
	{
		rCPU.dup();

		return 0;
	}

	template<typename T>
	short cpu_sin(CPU<T> &rCPU, const args_t&)
	{
		rCPU.sin();

		return 0;
	}

	template<typename T>
	short cpu_cos(CPU<T> &rCPU, const args_t&)
	{
		rCPU.cos();

		return 0;
	}

	template<typename T>
	short cpu_dump(CPU<T> &rCPU, const args_t&)
	{
		rCPU.dump();

		return 0;
	}

	template<typename T>
	short cpu_cmp(CPU<T> &rCPU, const args_t &crArgs)
	{
		rCPU.push(GetValue<T>(crArgs[0]), CPU<>::MemoryStorage::STACK);
		rCPU.push(GetValue<T>(crArgs[1]), CPU<>::MemoryStorage::STACK);

		return 0;
	}

	template<typename T>
	short cpu_jump(CPU<T>&, const args_t&)
	{
		return 1;
	}

	template<typename T>
	short cpu_je(CPU<T> &rCPU, const args_t&)
	{
		if(auto pair = rCPU.getPair(); pair.first == pair.second) 
			return 1;

		return 0;
	}

	template<typename T>
	short cpu_jne(CPU<T> &rCPU, const args_t&)
	{
		if (auto pair = rCPU.getPair(); pair.first != pair.second)
			return 1;

		return 0;
	}

	template<typename T>
	short cpu_ja(CPU<T> &rCPU, const args_t&)
	{
		if (auto pair = rCPU.getPair(); pair.first > pair.second)
			return 1;

		return 0;
	}

	template<typename T>
	short cpu_jae(CPU<T> &rCPU, const args_t&)
	{
		if (auto pair = rCPU.getPair(); pair.first >= pair.second)
			return 1;

		return 0;
	}

	template<typename T>
	short cpu_jb(CPU<T> &rCPU, const args_t&)
	{
		if (auto pair = rCPU.getPair(); pair.first < pair.second)
			return 1;

		return 0;
	}

	template<typename T>
	short cpu_jbe(CPU<T> &rCPU, const args_t&)
	{
		if (auto pair = rCPU.getPair(); pair.first <= pair.second)
			return 1;

		return 0;
	}

	template<typename T>
	short cpu_move(CPU<T> &rCPU, const args_t &crArgs)
	{
		bool isArg0Reg = NRegister::IsReg(crArgs[0]),
			isArg1Reg = NRegister::IsReg(crArgs[1]);

		if (isArg0Reg && isArg1Reg)
			rCPU.move(NRegister::MakeReg(crArgs[0]), NRegister::MakeReg(crArgs[1]));
		else if (!isArg0Reg && isArg1Reg)
			rCPU.move(GetValue<T>(crArgs[0]), NRegister::MakeReg(crArgs[1]));
		else
			std::cerr << "This was not realised yet\n";

		return 0;
	}

	template<typename T>
	short cpu_call(CPU<T>&, const args_t&)
	{
		return 2;
	}

	template<typename T>
	short cpu_ret(CPU<T> &rCPU, const args_t&)
	{
		auto ret_val = rCPU.top();

		rCPU.pop(CPU<>::MemoryStorage::STACK_FUNC_RET_ADDR);

		return static_cast<short>(ret_val);
	}

#pragma endregion

} // namespace NCpu::Commands

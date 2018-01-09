#pragma once

#include <string_view> // std::string_view
#include <array>       // std::array

// #include "CPU.hpp"

enum class Commands : unsigned
{
	undefined = 4294967295,

	push = 0,
	pop  = 2,

	add  = 3,
	sub  = 4,
	mul  = 5,
	div  = 6,
	sqrt = 7,
	dup  = 8,
	sin  = 9,
	cos  = 10,

	dump = 11,

	cmp  = 12,
	jump = 13,

	je   = 14,
	jne  = 15,
	ja   = 16,
	jae  = 17,
	jb   = 18,
	jbe  = 19,

	move = 20,

	call = 21,
	ret  = 22,

	end  = 23,

	NUM
};

class Command
{
public:	
	std::string_view  name;
	Commands          number;
	char              numOfArgs;

	constexpr explicit Command() :
		name("null"),
		number(Commands::undefined),
		numOfArgs('0')
	{ }

	constexpr Command(std::string_view str, Commands com, char args) :
		name(str),
		number(com),
		numOfArgs(args)
	{ }

	constexpr Command(const Command &crCommand) :
		name(crCommand.name),
		number(crCommand.number),
		numOfArgs(crCommand.numOfArgs)
	{ }

	Command &operator=(const Command&) = default;
	Command &operator=(Command&&)      = default;
};

/* constexpr */ std::array<Command, static_cast<size_t>(Commands::NUM)> cgTable =
{
	Command("push", Commands::push, '1'),
	Command("pop",  Commands::pop,  '1'),
	Command("add",  Commands::add,  '0'),
	Command("sub",  Commands::sub,  '0'),
	Command("mul",  Commands::mul,  '0'),
	Command("div",  Commands::div,  '0'),
	Command("sqrt", Commands::sqrt, '0'),
	Command("dup",  Commands::dup,  '0'),
	Command("sin",  Commands::sin,  '0'),
	Command("cos",  Commands::cos,  '0'),
	Command("dump", Commands::dump, '0'),
	Command("cmp",  Commands::cmp,  '2'),
	Command("jump", Commands::jump, '1'),
	Command("je",   Commands::je,   '1'),
	Command("jne",  Commands::jne,  '1'),
	Command("ja",   Commands::ja,   '1'),
	Command("jae",  Commands::jae,  '1'),
	Command("jb",   Commands::jb,   '1'),
	Command("jbe",  Commands::jbe,  '1'),
	Command("move", Commands::move, '2'),
	Command("call", Commands::call, '1'),
	Command("ret",  Commands::ret,  '0'),
	Command("end",  Commands::end,  '0')
};
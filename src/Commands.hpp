#pragma once

#include <string_view> // std::string_view
#include <array>       // std::array

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

class Command
{
public:	
	std::string_view  name;
	Commands          number;
	char              numOfArgs;

	constexpr explicit Command() :
		name(std::string_view("null", 4)),
		number(Commands::undefined),
		numOfArgs('0')
	{ }

	constexpr Command(const std::string_view &crStr, Commands com, char args) :
		name(crStr),
		number(com),
		numOfArgs(args)
	{ }

	constexpr Command(const Command &crCommand) :
		name(crCommand.name),
		number(crCommand.number),
		numOfArgs(crCommand.numOfArgs)
	{ }

	constexpr Command &operator=(const Command&) = default;
	constexpr Command &operator=(Command&&)      = default;
};

constexpr std::array<Command, static_cast<size_t>(Commands::NUM)> cgTable
{
	Command(std::string_view("push", 4), Commands::push, '1'),
	Command(std::string_view("pop",  3), Commands::pop,  '1'),
	Command(std::string_view("add",  3), Commands::add,  '0'),
	Command(std::string_view("sub",  3), Commands::sub,  '0'),
	Command(std::string_view("mul",  3), Commands::mul,  '0'),
	Command(std::string_view("div",  3), Commands::div,  '0'),
	Command(std::string_view("sqrt", 4), Commands::sqrt, '0'),
	Command(std::string_view("dup",  3), Commands::dup,  '0'),
	Command(std::string_view("sin",  3), Commands::sin,  '0'),
	Command(std::string_view("cos",  3), Commands::cos,  '0'),
	Command(std::string_view("dump", 4), Commands::dump, '0'),
	Command(std::string_view("cmp",  3), Commands::cmp,  '2'),
	Command(std::string_view("jump", 4), Commands::jump, '1'),
	Command(std::string_view("je",   2), Commands::je,   '1'),
	Command(std::string_view("jne",  3), Commands::jne,  '1'),
	Command(std::string_view("ja",   2), Commands::ja,   '1'),
	Command(std::string_view("jae",  3), Commands::jae,  '1'),
	Command(std::string_view("jb",   2), Commands::jb,   '1'),
	Command(std::string_view("jbe",  3), Commands::jbe,  '1'),
	Command(std::string_view("move", 4), Commands::move, '2'),
	Command(std::string_view("call", 4), Commands::call, '1'),
	Command(std::string_view("ret",  3), Commands::ret,  '0'),
	Command(std::string_view("end",  3), Commands::end,  '0')
};
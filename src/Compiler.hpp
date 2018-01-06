#pragma once

#include <fstream>
#include <sstream>

#include "Parser.hpp"
#include "CPU.hpp"
#include "Wrap4BinaryIO.hpp"

namespace NCompiler
{
#pragma region Usings

	using namespace NParser;

	using NCpu::REG;
	using NCpu::CPU;

#pragma endregion

	template<typename T = INT>
	class Compiler final
	{
		CPU<T> cpu_;

		T getValue(CRSTRING) const;

		REG  makeReg(CRSTRING) const;
		BOOL   isReg(CRSTRING) const;

	public:
		enum class commands_ : UNSIGNED
		{
			undefined,

			push = 1,
			pop = 2,

			add = 3,
			sub = 4,
			mul = 5,
			div = 6,
			sqrt = 7,
			dup = 8,
			sin = 9,
			cos = 10,

			dump = 11,

			cmp = 12,
			jump = 13,

			je = 14,
			jne = 15,
			ja = 16,
			jae = 17,
			jb = 18,
			jbe = 19,

			move = 20,

			call = 21,
			ret = 22,

			end = 23
		};

		explicit Compiler()       _NOEXCEPT;
		Compiler(CONST Compiler&);
		Compiler(Compiler&&)      _NOEXCEPT;
		~Compiler();

		Compiler<T> &operator=(CONST Compiler&);
		Compiler<T> &operator=(Compiler&&);

		BOOL text2com(CRSTRING) const;
		BOOL text2bin(CRSTRING) const;
		BOOL com2bin(CRSTRING)  const;

		BOOL fromTextFile(CRSTRING);
		BOOL fromComFile(CRSTRING);
		BOOL fromBinTextFile(CRSTRING);
		BOOL fromBinComFile(CRSTRING);
	};

	template<typename T>
	inline Compiler<T>::Compiler() _NOEXCEPT :
	cpu_()
	{ }

	template<typename T>
	inline Compiler<T>::Compiler(CONST Compiler &crComp) :
		cpu_(crComp.cpu_)
	{ }

	template<typename T>
	inline Compiler<T>::Compiler(Compiler &&rrComp) _NOEXCEPT :
		cpu_(rrComp.cpu_)
	{ }

	template<typename T>
	inline Compiler<T>::~Compiler()
	{ }

	template<typename T>
	T Compiler<T>::getValue(CRSTRING str) const
	{
		std::stringstream sstr(str);
		if (str[0] == '[')
		{
			CHAR c = 'A';
			sstr >> c;

			assert(c == '[');
		}

		T val = NULL;
		sstr >> val;

		return (val ? val : static_cast<T>(commands_::undefined));
	}

	template<typename T>
	REG Compiler<T>::makeReg(CRSTRING str) const
	{
		if (str == "ax" || str == "[ax]") return REG::AX;
		else if (str == "bx" || str == "[bx]") return REG::BX;
		else if (str == "cx" || str == "[cx]") return REG::CX;
		else if (str == "dx" || str == "[dx]") return REG::DX;
		else if (str == "ex" || str == "[ex]") return REG::EX;
		else if (str == "sp" || str == "[sp]") return REG::SP;

		else                                   return REG::NUM;
	}

	template<typename T>
	inline BOOL Compiler<T>::isReg(CRSTRING val) const
	{
		return (makeReg(val) != REG::NUM);
	}

	template<typename T>
	inline Compiler<T> &Compiler<T>::operator=(CONST Compiler &crComp)
	{
		if (this != &crComp) cpu_ = crComp.cpu_;

		return (*this);
	}

	template<typename T>
	inline Compiler<T> &Compiler<T>::operator=(Compiler &&rrComp)
	{
		assert(this != &rrComp);

		cpu_ = std::move(rrComp.cpu_);

		return (*this);
	}

#pragma region Functions Compiler<>::toSomeFile

	template<typename T>
	BOOL Compiler<T>::text2com(CRSTRING filename) const
	{
		std::ifstream input(filename + ".txt");
		if (!input.is_open())
		{
			NDebugger::Error("Cannot open file: " + filename);

			return FALSE;
		}

		std::string outputFile = filename + "Com" + ".txt";
		std::ofstream output(outputFile);
		if (!output.is_open())
		{
			NDebugger::Error("Cannot open file: " + outputFile + "Com");

			input.close();

			return FALSE;
		}

		while (!input.eof())
		{
			CHAR tmp[MAX_LINE_LENGTH] = {};
			input.getline(tmp, MAX_LINE_LENGTH, '\n');

			Operation op(std::move(ParseCode(tmp)));

			if (!op.cmd.length()) continue;  // Have to check everything so that there are no undefined commands

			else if (op.cmd == "push") output << static_cast<UNSIGNED>(commands_::push) << " " << op.args[0];
			else if (op.cmd == "pop")  output << static_cast<UNSIGNED>(commands_::pop) << " " << op.args[0];

			else if (op.cmd == "add")  output << static_cast<UNSIGNED>(commands_::add);
			else if (op.cmd == "sub")  output << static_cast<UNSIGNED>(commands_::sub);
			else if (op.cmd == "mul")  output << static_cast<UNSIGNED>(commands_::mul);
			else if (op.cmd == "div")  output << static_cast<UNSIGNED>(commands_::div);
			else if (op.cmd == "dup")  output << static_cast<UNSIGNED>(commands_::dup);
			else if (op.cmd == "sin")  output << static_cast<UNSIGNED>(commands_::sin);
			else if (op.cmd == "cos")  output << static_cast<UNSIGNED>(commands_::cos);
			else if (op.cmd == "sqrt") output << static_cast<UNSIGNED>(commands_::sqrt);

			else if (op.cmd == "dump") output << static_cast<UNSIGNED>(commands_::dump);

			else if (op.cmd == "cmp")  output << static_cast<UNSIGNED>(commands_::cmp) << " " << op.args[0] << " " << op.args[1];
			else if (op.cmd == "jump") output << static_cast<UNSIGNED>(commands_::jump) << " " << op.args[0];

			else if (op.cmd == "je")   output << static_cast<UNSIGNED>(commands_::je) << " " << op.args[0];
			else if (op.cmd == "jne")  output << static_cast<UNSIGNED>(commands_::jne) << " " << op.args[0];
			else if (op.cmd == "ja")   output << static_cast<UNSIGNED>(commands_::ja) << " " << op.args[0];
			else if (op.cmd == "jae")  output << static_cast<UNSIGNED>(commands_::jae) << " " << op.args[0];
			else if (op.cmd == "jb")   output << static_cast<UNSIGNED>(commands_::jb) << " " << op.args[0];
			else if (op.cmd == "jbe")  output << static_cast<UNSIGNED>(commands_::jbe) << " " << op.args[0];

			else if (op.cmd == "move") output << static_cast<UNSIGNED>(commands_::move) << " " << op.args[0] << " " << op.args[1];

			else if (op.cmd == "call") output << static_cast<UNSIGNED>(commands_::call) << " " << op.args[0];
			else if (op.cmd == "ret")  output << static_cast<UNSIGNED>(commands_::ret);

			else if (op.cmd == "end")  output << static_cast<UNSIGNED>(commands_::end);

			else if (op.cmd[0] == ':' || op.cmd[op.cmd.length() - 1] == ':') output << op.cmd;

			else
			{
				NDebugger::Error("Unknown command: " + std::string(op.cmd));

				output.close();
				input.close();

				return FALSE;
			}

			output << std::endl;
		}

		output.close();
		input.close();

		return TRUE;
	}

	template<typename T>
	BOOL Compiler<T>::text2bin(CRSTRING filename) const
	{
		std::ifstream input(filename + ".txt");
		if (!input.is_open())
		{
			NDebugger::Error("Cannot open file: " + filename);

			return FALSE;
		}

		std::string outputFile = filename + "BinText" + ".txt";
		std::ofstream output(outputFile, std::ios::binary);
		if (!output.is_open())
		{
			NDebugger::Error("Cannot open file: " + outputFile + "BinText");

			input.close();

			return FALSE;
		}

#pragma warning(push)
#pragma warning(disable:4238) // Standard extension: use of the right-hand class value as a left-handed value in defines

#define WRITE_STRING(str) Wrap4BinaryIO<std::string>(str)
#define ARG(index)        WRITE_STRING(op.args[index])

		while (!input.eof())
		{
			CHAR tmp[MAX_LINE_LENGTH] = {};
			input.getline(tmp, MAX_LINE_LENGTH, '\n');

			Operation op(std::move(ParseCode(tmp)));
			if (!op.cmd.length() || op.cmd == "null") continue; // Have to check everything so that there are no undefined commands

			else if (op.cmd == "push") output << WRITE_STRING(op.cmd) << ARG(0);
			else if (op.cmd == "pop")
			{
				output << WRITE_STRING(op.cmd);

				if (op.args[0].length()) output << ARG(0);
			}

			else if (op.cmd == "add")  output << WRITE_STRING(op.cmd);
			else if (op.cmd == "sub")  output << WRITE_STRING(op.cmd);
			else if (op.cmd == "mul")  output << WRITE_STRING(op.cmd);
			else if (op.cmd == "div")  output << WRITE_STRING(op.cmd);
			else if (op.cmd == "dup")  output << WRITE_STRING(op.cmd);
			else if (op.cmd == "sin")  output << WRITE_STRING(op.cmd);
			else if (op.cmd == "cos")  output << WRITE_STRING(op.cmd);
			else if (op.cmd == "sqrt") output << WRITE_STRING(op.cmd);

			else if (op.cmd == "dump") output << WRITE_STRING(op.cmd);

			else if (op.cmd == "cmp")  output << WRITE_STRING(op.cmd) << ARG(0) << ARG(1);
			else if (op.cmd == "jump") output << WRITE_STRING(op.cmd) << ARG(0);

			else if (op.cmd == "je")   output << WRITE_STRING(op.cmd) << ARG(0);
			else if (op.cmd == "jne")  output << WRITE_STRING(op.cmd) << ARG(0);
			else if (op.cmd == "ja")   output << WRITE_STRING(op.cmd) << ARG(0);
			else if (op.cmd == "jae")  output << WRITE_STRING(op.cmd) << ARG(0);
			else if (op.cmd == "jb")   output << WRITE_STRING(op.cmd) << ARG(0);
			else if (op.cmd == "jbe")  output << WRITE_STRING(op.cmd) << ARG(0);

			else if (op.cmd == "move") output << WRITE_STRING(op.cmd) << ARG(0) << ARG(1);

			else if (op.cmd == "call") output << WRITE_STRING(op.cmd) << ARG(0);
			else if (op.cmd == "ret")  output << WRITE_STRING(op.cmd);

			else if (op.cmd == "end")  output << WRITE_STRING(op.cmd);

			else if (op.cmd[0] == ':' || op.cmd[op.cmd.length() - 1] == ':') output << WRITE_STRING(op.cmd);

			else
			{
				NDebugger::Error("Unknown command: " + op.cmd);

				output.close();
				input.close();

				return FALSE;
			}
		}

#undef ARG
#undef WRITE_STRING

#pragma warning(pop)

		output.close();
		input.close();

		return TRUE;
	}

	template<typename T>
	BOOL Compiler<T>::com2bin(CRSTRING filename) const
	{
		std::ifstream input(filename + "Com.txt");
		if (!input.is_open())
		{
			NDebugger::Error("Cannot open file: " + filename);

			return FALSE;
		}

		std::string outputFile = filename + "BinCom" + ".txt";
		std::ofstream output(outputFile, std::ios::binary);
		if (!output.is_open())
		{
			NDebugger::Error("Cannot open file: " + outputFile + "BinCom");

			input.close();

			return FALSE;
		}

#pragma warning(push)
#pragma warning(disable:4238) // Standard extension: use of the right-hand class value as a left-handed value in defines

#define WRITE_STRING(str) Wrap4BinaryIO<std::string>(str)
#define ARG(index)        WRITE_STRING(op.args[index])

		while (!input.eof())
		{
			CHAR tmp[MAX_LINE_LENGTH] = {};
			input.getline(tmp, MAX_LINE_LENGTH, '\n');

			Operation op(std::move(ParseCode(tmp)));
			if (!op.cmd.length()) continue;  // Have to check everything so that there are no undefined commands

			else if (op.cmd[0] == ':' || op.cmd[op.cmd.length() - 1] == ':')
			{
				output << WRITE_STRING(op.cmd);

				continue;
			}

			INT command = std::stoi(op.cmd);
			if      (command == static_cast<INT>(commands_::push)) output << WRITE_STRING(op.cmd) << ARG(0);
			else if (command == static_cast<INT>(commands_::pop))
			{
				output << WRITE_STRING(op.cmd);

				if (op.args[0].length()) output << WRITE_STRING(op.args[0]);
			}

			else if (command == static_cast<INT>(commands_::add))  output << WRITE_STRING(op.cmd);
			else if (command == static_cast<INT>(commands_::sub))  output << WRITE_STRING(op.cmd);
			else if (command == static_cast<INT>(commands_::div))  output << WRITE_STRING(op.cmd);
			else if (command == static_cast<INT>(commands_::mul))  output << WRITE_STRING(op.cmd);
			else if (command == static_cast<INT>(commands_::dup))  output << WRITE_STRING(op.cmd);
			else if (command == static_cast<INT>(commands_::sin))  output << WRITE_STRING(op.cmd);
			else if (command == static_cast<INT>(commands_::cos))  output << WRITE_STRING(op.cmd);
			else if (command == static_cast<INT>(commands_::sqrt)) output << WRITE_STRING(op.cmd);

			else if (command == static_cast<INT>(commands_::dump)) output << WRITE_STRING(op.cmd);

			else if (command == static_cast<INT>(commands_::cmp))  output << WRITE_STRING(op.cmd) << ARG(0) << ARG(1);
			else if (command == static_cast<INT>(commands_::jump)) output << WRITE_STRING(op.cmd) << ARG(0);

			else if (command == static_cast<INT>(commands_::je))  output << WRITE_STRING(op.cmd) << ARG(0);
			else if (command == static_cast<INT>(commands_::jne)) output << WRITE_STRING(op.cmd) << ARG(0);
			else if (command == static_cast<INT>(commands_::ja))  output << WRITE_STRING(op.cmd) << ARG(0);
			else if (command == static_cast<INT>(commands_::jae)) output << WRITE_STRING(op.cmd) << ARG(0);
			else if (command == static_cast<INT>(commands_::jb))  output << WRITE_STRING(op.cmd) << ARG(0);
			else if (command == static_cast<INT>(commands_::jbe)) output << WRITE_STRING(op.cmd) << ARG(0);

			else if (command == static_cast<INT>(commands_::move)) output << WRITE_STRING(op.cmd) << ARG(0) << ARG(1);

			else if (command == static_cast<INT>(commands_::call)) output << WRITE_STRING(op.cmd) << ARG(0);
			else if (command == static_cast<INT>(commands_::ret))  output << WRITE_STRING(op.cmd);

			else if (command == static_cast<INT>(commands_::end)) output << WRITE_STRING(op.cmd);

			else
			{
				NDebugger::Error("Unknown command: " + op.cmd);

				input.close();
				output.close();

				return FALSE;
			}
		}

#undef ARG
#undef WRITE_STRING

#pragma warning(pop)

		output.close();
		input.close();

		return TRUE;
	}

#pragma endregion

#pragma region Functions Compiler<>::fromSomeFile

	template<typename T>
	BOOL Compiler<T>::fromTextFile(CRSTRING filename)
	{
		std::ifstream file(filename + ".txt");
		if (!file.is_open())
		{
			NDebugger::Error("Cannot open file: " + filename);

			return FALSE;
		}

		while (!file.eof())
		{
			CHAR tmp[MAX_LINE_LENGTH] = { };
			file.getline(tmp, MAX_LINE_LENGTH, '\n');

			Operation op(std::move(ParseCode(tmp)));
			if (!op.cmd.length() || op.cmd[0] == ':' || op.cmd == "null") continue; // Skip the label or zero-line

			else if (op.cmd[op.cmd.length() - 1] == ':') // Skip the function
			{
				do
				{
					file.getline(tmp, MAX_LINE_LENGTH, '\n');
					op = std::move(ParseCode(tmp));
				} while (op.cmd != "ret");

				continue;
			}

			else if (op.cmd == "push")
			{
				auto val = getValue(op.args[0]);
				auto reg = makeReg(op.args[0]);

				if (op.args[0][0] == '[' && reg != REG::NUM) cpu_.push(reg, CPU<>::MemoryStorage::RAM);
				else if (op.args[0][0] == '[' && reg == REG::NUM) cpu_.push(val, CPU<>::MemoryStorage::RAM);
				else if (reg != REG::NUM) cpu_.push(reg, CPU<>::MemoryStorage::STACK);
				else		                                      cpu_.push(val, CPU<>::MemoryStorage::STACK);
			}
			else if (op.cmd == "pop")
			{
				if (op.args[0][0] == '[') cpu_.pop(CPU<>::MemoryStorage::RAM);
				else                      cpu_.pop(CPU<>::MemoryStorage::STACK);
			}

			else if (op.cmd == "add") cpu_.add();
			else if (op.cmd == "sub") cpu_.sub();
			else if (op.cmd == "mul") cpu_.mul();
			else if (op.cmd == "div") cpu_.div();
			else if (op.cmd == "dup") cpu_.dup();
			else if (op.cmd == "sin") cpu_.sin();
			else if (op.cmd == "cos") cpu_.cos();
			else if (op.cmd == "sqrt") cpu_.sqrt();

			else if (op.cmd == "dump") cpu_.dump();

			else if (op.cmd == "cmp")  cpu_.push(getValue(op.args[0]), CPU<>::MemoryStorage::STACK),
				cpu_.push(getValue(op.args[1]), CPU<>::MemoryStorage::STACK);
			else if (op.cmd == "jump") Move2Label(file, ':' + op.args[0]);

			else if (op.cmd == "je") { auto pair = cpu_.getPair(); if (pair.first == pair.second) Move2Label(file, ':' + op.args[0]); }
			else if (op.cmd == "jne") { auto pair = cpu_.getPair(); if (pair.first != pair.second) Move2Label(file, ':' + op.args[0]); }
			else if (op.cmd == "ja") { auto pair = cpu_.getPair(); if (pair.first > pair.second) Move2Label(file, ':' + op.args[0]); }
			else if (op.cmd == "jae") { auto pair = cpu_.getPair(); if (pair.first >= pair.second) Move2Label(file, ':' + op.args[0]); }
			else if (op.cmd == "jb") { auto pair = cpu_.getPair(); if (pair.first < pair.second) Move2Label(file, ':' + op.args[0]); }
			else if (op.cmd == "jbe") { auto pair = cpu_.getPair(); if (pair.first <= pair.second) Move2Label(file, ':' + op.args[0]); }

			else if (op.cmd == "move")
			{
				BOOL isArg0Reg = isReg(op.args[0]),
					isArg1Reg = isReg(op.args[1]);

				if (isArg0Reg &&  isArg1Reg) cpu_.move(makeReg(op.args[0]), makeReg(op.args[1]));
				else if (!isArg0Reg &&  isArg1Reg) cpu_.move(getValue(op.args[0]), makeReg(op.args[1]));
			}

			else if (op.cmd == "call")
			{
				cpu_.push(file.tellg());
				Move2Label(file, op.args[0] + ':');
			}
			else if (op.cmd == "ret")
			{
				file.seekg(cpu_.top());
				cpu_.pop(CPU<>::MemoryStorage::STACK_FUNC_RET_ADDR);
			}

			else if (op.cmd == "end") break;

			else
			{
				NDebugger::Error("Unknown command: " + op.cmd);

				file.close();

				return FALSE;
			}
		}

		file.close();

		return TRUE;
	}

	template<typename T>
	BOOL Compiler<T>::fromComFile(CRSTRING filename)
	{
		std::ifstream file(filename + "Com.txt");
		if (!file.is_open())
		{
			NDebugger::Error("Cannot open file: " + filename + "Com");

			return FALSE;
		}

		while (!file.eof())
		{
			CHAR tmp[MAX_LINE_LENGTH] = { };
			file.getline(tmp, MAX_LINE_LENGTH, '\n');

			Operation op(std::move(ParseCode(tmp)));

			if (!op.cmd.length() || op.cmd[0] == ':' || op.cmd == "null") continue; // Skip the label or zero-line

			else if (op.cmd[op.cmd.length() - 1] == ':') // Skip the function
			{
				do
				{
					file.getline(tmp, MAX_LINE_LENGTH, '\n');
					op = std::move(ParseCode(tmp));
				} while (std::stoi(op.cmd) != static_cast<INT>(commands_::ret));

				continue;
			}

			INT command = std::stoi(op.cmd);
			if (command == static_cast<INT>(commands_::push))
			{
				auto val = getValue(op.args[0]);
				auto reg = makeReg(op.args[0]);

				if (op.args[0][0] == '[' && reg != REG::NUM) cpu_.push(reg, CPU<>::MemoryStorage::RAM);
				else if (op.args[0][0] == '[' && reg == REG::NUM) cpu_.push(val, CPU<>::MemoryStorage::RAM);
				else if (reg != REG::NUM) cpu_.push(reg, CPU<>::MemoryStorage::STACK);
				else		                                      cpu_.push(val, CPU<>::MemoryStorage::STACK);
			}
			else if (command == static_cast<INT>(commands_::pop))
			{
				if (op.args[0][0] == '[') cpu_.pop(CPU<>::MemoryStorage::RAM);
				else                      cpu_.pop(CPU<>::MemoryStorage::STACK);
			}

			else if (command == static_cast<INT>(commands_::add))  cpu_.add();
			else if (command == static_cast<INT>(commands_::sub))  cpu_.sub();
			else if (command == static_cast<INT>(commands_::div))  cpu_.div();
			else if (command == static_cast<INT>(commands_::dup))  cpu_.dup();
			else if (command == static_cast<INT>(commands_::sin))  cpu_.sin();
			else if (command == static_cast<INT>(commands_::cos))  cpu_.cos();
			else if (command == static_cast<INT>(commands_::sqrt)) cpu_.sqrt();

			else if (command == static_cast<INT>(commands_::dump)) cpu_.dump();

			else if (command == static_cast<INT>(commands_::cmp))  cpu_.push(getValue(op.args[0]), CPU<>::MemoryStorage::STACK),
				cpu_.push(getValue(op.args[1]), CPU<>::MemoryStorage::STACK);
			else if (command == static_cast<INT>(commands_::jump)) Move2Label(file, ':' + op.args[0]);

			else if (command == static_cast<INT>(commands_::je)) { auto pair = cpu_.getPair(); if (pair.first == pair.second) Move2Label(file, ':' + op.args[0]); }
			else if (command == static_cast<INT>(commands_::jne)) { auto pair = cpu_.getPair(); if (pair.first != pair.second) Move2Label(file, ':' + op.args[0]); }
			else if (command == static_cast<INT>(commands_::ja)) { auto pair = cpu_.getPair(); if (pair.first > pair.second) Move2Label(file, ':' + op.args[0]); }
			else if (command == static_cast<INT>(commands_::jae)) { auto pair = cpu_.getPair(); if (pair.first >= pair.second) Move2Label(file, ':' + op.args[0]); }
			else if (command == static_cast<INT>(commands_::jb)) { auto pair = cpu_.getPair(); if (pair.first < pair.second) Move2Label(file, ':' + op.args[0]); }
			else if (command == static_cast<INT>(commands_::jbe)) { auto pair = cpu_.getPair(); if (pair.first <= pair.second) Move2Label(file, ':' + op.args[0]); }

			else if (command == static_cast<INT>(commands_::move))
			{
				BOOL isArg0Reg = isReg(op.args[0]),
					isArg1Reg = isReg(op.args[1]);

				if (isArg0Reg &&  isArg1Reg) cpu_.move(makeReg(op.args[0]), makeReg(op.args[1]));
				else if (!isArg0Reg &&  isArg1Reg) cpu_.move(getValue(op.args[0]), makeReg(op.args[1]));
			}

			else if (command == static_cast<INT>(commands_::call))
			{
				cpu_.push(file.tellg());
				Move2Label(file, op.args[0] + ':');
			}
			else if (command == static_cast<INT>(commands_::ret))
			{
				file.seekg(cpu_.top());
				cpu_.pop(CPU<>::MemoryStorage::STACK_FUNC_RET_ADDR);
			}

			else if (command == static_cast<INT>(commands_::end)) break;

			else
			{
				NDebugger::Error("Unknown command: " + command);

				file.close();

				return FALSE;
			}
		}

		file.close();

		return TRUE;
	}

	template<typename T>
	BOOL Compiler<T>::fromBinTextFile(CRSTRING filename)
	{
		std::ifstream file(filename + "BinText.txt", std::ios::binary);
		if (!file.is_open())
		{
			NDebugger::Error("Cannot open file: " + filename + "BinText");

			return FALSE;
		}

#define COMMAND command.operator CRSTRING()

		BOOL skipCommand = FALSE;
		while (!file.eof())
		{
			Wrap4BinaryIO<std::string> command;
			file >> command;

			std::cout << command << std::endl;
			
			if (!COMMAND.length() || COMMAND[0] == ':') continue; // Skip the label

			else if (COMMAND[COMMAND.length() - 1] == ':') skipCommand = TRUE; // Skip the function

			else if (COMMAND == "push")
			{
				file >> command;

				if (skipCommand) continue;

				auto val = getValue(command);
				auto reg = makeReg(command);

				if      (COMMAND[0] == '[' && reg != REG::NUM) cpu_.push(reg, CPU<>::MemoryStorage::RAM);
				else if (COMMAND[0] == '[' && reg == REG::NUM) cpu_.push(val, CPU<>::MemoryStorage::RAM);
				else if (                     reg != REG::NUM) cpu_.push(reg, CPU<>::MemoryStorage::STACK);
				else		                                   cpu_.push(val, CPU<>::MemoryStorage::STACK);
			}
			else if (COMMAND == "pop")
			{
				auto oldPos = file.tellg();

				file >> command;

				if (COMMAND[0] == '[') { if (!skipCommand) cpu_.pop(CPU<>::MemoryStorage::RAM); }
				else
				{
					if (!skipCommand) cpu_.pop(CPU<>::MemoryStorage::STACK);

					file.seekg(oldPos);
				}
			}

			else if (COMMAND == "add") { if (!skipCommand) cpu_.add(); }
			else if (COMMAND == "sub") { if (!skipCommand) cpu_.sub(); }
			else if (COMMAND == "mul") { if (!skipCommand) cpu_.mul(); }
			else if (COMMAND == "div") { if (!skipCommand) cpu_.div(); }
			else if (COMMAND == "dup") { if (!skipCommand) cpu_.dup(); }
			else if (COMMAND == "sin") { if (!skipCommand) cpu_.sin(); }
			else if (COMMAND == "cos") { if (!skipCommand) cpu_.cos(); }
			else if (COMMAND == "sqrt") { if (!skipCommand) cpu_.sqrt(); }

			else if (COMMAND == "dump") { if (!skipCommand) cpu_.dump(); }


			else if (COMMAND == "cmp")
			{
				file >> command; //-V760
				if (!skipCommand)
				{
					auto reg = makeReg(command);

					if (reg != REG::NUM) cpu_.push(reg, CPU<>::MemoryStorage::STACK);
					else                 cpu_.push(getValue(command), CPU<>::MemoryStorage::STACK);
				}

				file >> command;
				if (!skipCommand) 
				{
					auto reg = makeReg(command);

					if (reg != REG::NUM) cpu_.push(reg, CPU<>::MemoryStorage::STACK);
					else                 cpu_.push(getValue(command), CPU<>::MemoryStorage::STACK);
				}
			}
			else if (COMMAND == "jump") { file >> command; if (skipCommand) continue; Move2LabelBin(file, command); }

			else if (COMMAND == "je")  { file >> command; if (skipCommand) continue; auto pair = cpu_.getPair(); if (pair.first == pair.second) Move2LabelBin(file, command); }
			else if (COMMAND == "jne") { file >> command; if (skipCommand) continue; auto pair = cpu_.getPair(); if (pair.first != pair.second) Move2LabelBin(file, command); }
			else if (COMMAND == "ja")  { file >> command; if (skipCommand) continue; auto pair = cpu_.getPair(); if (pair.first  > pair.second) Move2LabelBin(file, command); }
			else if (COMMAND == "jae") { file >> command; if (skipCommand) continue; auto pair = cpu_.getPair(); if (pair.first >= pair.second) Move2LabelBin(file, command); }
			else if (COMMAND == "jb")  { file >> command; if (skipCommand) continue; auto pair = cpu_.getPair(); if (pair.first  < pair.second) Move2LabelBin(file, command); }
			else if (COMMAND == "jbe") { file >> command; if (skipCommand) continue; auto pair = cpu_.getPair(); if (pair.first <= pair.second) Move2LabelBin(file, command); }

			else if (COMMAND == "move")
			{
				Wrap4BinaryIO<std::string> arg0,
					                       arg1;
				file >> arg0 >> arg1;

				if (skipCommand) continue;

				BOOL isArg0Reg = isReg(arg0),
					isArg1Reg = isReg(arg1);

				if (isArg0Reg &&  isArg1Reg) cpu_.move(makeReg(arg0), makeReg(arg1));
				else if (!isArg0Reg &&  isArg1Reg) cpu_.move(getValue(arg0), makeReg(arg1));
			}

			else if (COMMAND == "call")
			{
				file >> command;

				if (skipCommand) continue;

				cpu_.push(file.tellg());
				Move2LabelBin(file, COMMAND + ':');
			}
			else if (COMMAND == "ret")
			{
				if (skipCommand)
				{
					skipCommand = FALSE;

					continue;
				}

				file.seekg(cpu_.top());
				cpu_.pop(CPU<>::MemoryStorage::STACK_FUNC_RET_ADDR);
			}

			else if (COMMAND == "end") if (!skipCommand) break;

			else
			{
				NDebugger::Error("Unknown command: " + COMMAND);

				file.close();

				return FALSE;
			}
		}

#undef COMMAND

		file.close();

		return TRUE;
	}

	template<typename T>
	BOOL Compiler<T>::fromBinComFile(CRSTRING filename)
	{
		std::ifstream file(filename + "BinCom.txt", std::ios::binary);
		if (!file.is_open())
		{
			NDebugger::Error("Cannot open file: " + filename + "BinCom");

			return FALSE;
		}

#define COMMAND command.operator CRSTRING()

		BOOL skipCommand = FALSE;
		while (!file.eof())
		{
			Wrap4BinaryIO<std::string> command;
			file >> command;
			
			std::cout << command << std::endl;

			if (!COMMAND.length() || COMMAND[0] == ':') continue; // Skip the label

			else if (COMMAND[COMMAND.length() - 1] == ':') { skipCommand = TRUE; continue; }

			INT comInNum = std::stoi(COMMAND);
			if      (comInNum == static_cast<INT>(commands_::push))
			{
				file >> command;
				
				if (skipCommand) continue;

				auto val = getValue(COMMAND);
				auto reg = makeReg(COMMAND);

				if      (COMMAND[0] == '[' && reg != REG::NUM) cpu_.push(reg, CPU<>::MemoryStorage::RAM);
				else if (COMMAND[0] == '[' && reg == REG::NUM) cpu_.push(val, CPU<>::MemoryStorage::RAM);
				else if (                     reg != REG::NUM) cpu_.push(reg, CPU<>::MemoryStorage::STACK);
				else		                                   cpu_.push(val, CPU<>::MemoryStorage::STACK);
			}
			else if (comInNum == static_cast<INT>(commands_::pop))
			{
				auto oldPos = file.tellg();

				file >> command;
				
				if (COMMAND[0] == '[') { if (!skipCommand) cpu_.pop(CPU<>::MemoryStorage::RAM); }
				else
				{
					if (!skipCommand) cpu_.pop(CPU<>::MemoryStorage::STACK), file.seekg(std::ios::cur - COMMAND.length());

					file.seekg(oldPos);
				}
			}

			else if (comInNum == static_cast<INT>(commands_::add))  { if (!skipCommand) cpu_.add(); }
			else if (comInNum == static_cast<INT>(commands_::sub))  { if (!skipCommand) cpu_.sub(); }
			else if (comInNum == static_cast<INT>(commands_::mul))  { if (!skipCommand) cpu_.mul(); }
			else if (comInNum == static_cast<INT>(commands_::div))  { if (!skipCommand) cpu_.div(); }
			else if (comInNum == static_cast<INT>(commands_::dup))  { if (!skipCommand) cpu_.dup(); }
			else if (comInNum == static_cast<INT>(commands_::sin))  { if (!skipCommand) cpu_.sin(); }
			else if (comInNum == static_cast<INT>(commands_::cos))  { if (!skipCommand) cpu_.cos(); }
			else if (comInNum == static_cast<INT>(commands_::sqrt)) { if (!skipCommand) cpu_.sqrt(); }

			else if (comInNum == static_cast<INT>(commands_::dump)) { if (!skipCommand) cpu_.dump(); }

			else if (comInNum == static_cast<INT>(commands_::cmp))
			{
				file >> command; 
				if (!skipCommand)
				{
					auto reg = makeReg(command);

					if(reg != REG::NUM) cpu_.push(reg, CPU<>::MemoryStorage::STACK);
					else                cpu_.push(getValue(command), CPU<>::MemoryStorage::STACK);
				}

				file >> command;
				if (!skipCommand)
				{
					auto reg = makeReg(command);
					
					if (reg != REG::NUM) cpu_.push(reg, CPU<>::MemoryStorage::STACK);
					else                 cpu_.push(getValue(command), CPU<>::MemoryStorage::STACK);
				}
			}
			else if (comInNum == static_cast<INT>(commands_::jump)) { file >> command; if (skipCommand) continue; Move2LabelBin(file, command); }

			else if (comInNum == static_cast<INT>(commands_::je))  { file >> command; if (skipCommand) continue; auto pair = cpu_.getPair(); if (pair.first == pair.second) Move2LabelBin(file, command); }
			else if (comInNum == static_cast<INT>(commands_::jne)) { file >> command; if (skipCommand) continue; auto pair = cpu_.getPair(); if (pair.first != pair.second) Move2LabelBin(file, command); }
			else if (comInNum == static_cast<INT>(commands_::ja))  { file >> command; if (skipCommand) continue; auto pair = cpu_.getPair(); if (pair.first  > pair.second) Move2LabelBin(file, command); }
			else if (comInNum == static_cast<INT>(commands_::jae)) { file >> command; if (skipCommand) continue; auto pair = cpu_.getPair(); if (pair.first >= pair.second) Move2LabelBin(file, command); }
			else if (comInNum == static_cast<INT>(commands_::jb))  { file >> command; if (skipCommand) continue; auto pair = cpu_.getPair(); if (pair.first  < pair.second) Move2LabelBin(file, command); }
			else if (comInNum == static_cast<INT>(commands_::jbe)) { file >> command; if (skipCommand) continue; auto pair = cpu_.getPair(); if (pair.first <= pair.second) Move2LabelBin(file, command); }

			else if (comInNum == static_cast<INT>(commands_::move))
			{
				Wrap4BinaryIO<std::string> arg0,
					                       arg1;
				file >> arg0 >> arg1;

				if (skipCommand) continue;

				BOOL isArg0Reg = isReg(arg0),
					 isArg1Reg = isReg(arg1);

				if      ( isArg0Reg &&  isArg1Reg) cpu_.move(makeReg(arg0), makeReg(arg1));
				else if (!isArg0Reg &&  isArg1Reg) cpu_.move(getValue(arg0), makeReg(arg1));
			}

			else if (comInNum == static_cast<INT>(commands_::call))
			{
				file >> command;
	
				if (skipCommand) continue;

				cpu_.push(file.tellg());
				Move2LabelBin(file, COMMAND + ':');
			}
			else if (comInNum == static_cast<INT>(commands_::ret))
			{
				if (skipCommand)
				{
					skipCommand = FALSE;

					continue;
				}

				file.seekg(cpu_.top());
				cpu_.pop(CPU<>::MemoryStorage::STACK_FUNC_RET_ADDR);
			}

			else if (comInNum == static_cast<INT>(commands_::end)) { if (!skipCommand) break; }

			else
			{
				NDebugger::Error("Unknown command: " + comInNum);

				file.close();

				return FALSE;
			}
		}

#undef COMMAND

		file.close();

		return TRUE;
	}

#pragma endregion
} // NCompiler

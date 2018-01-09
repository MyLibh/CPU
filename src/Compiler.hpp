#pragma once

#include <fstream>
#include <sstream>

#include "Parser.hpp"
#include "CPU.hpp"
#include "Wrap4BinaryIO.hpp"
#include "Commands.hpp"

namespace NCompiler
{
#pragma region Usings

	using namespace NParser;

	using NCpu::REG;
	using NCpu::CPU;

#pragma endregion

	template<typename T = int>
	class Compiler final
	{
		T getValue(std::string_view) const;

		REG  makeReg(std::string_view) const;
		bool   isReg(std::string_view) const;

	public:
		explicit Compiler()       noexcept;
		Compiler(const Compiler&) noexcept;
		Compiler(Compiler&&)      noexcept;
		~Compiler();

		Compiler<T> &operator=(const Compiler&) noexcept;
		Compiler<T> &operator=(Compiler&&)      noexcept;

		bool text2com(const std::string&) const;
		bool text2bin(const std::string&) const;
		bool com2bin(const std::string&)  const;

		bool fromTextFile(const std::string&);
		bool fromComFile(const std::string&);
		bool fromBinTextFile(const std::string&);
		bool fromBinComFile(const std::string&);

	private:
		CPU<T> cpu_;
	};

	template<typename T>
	inline Compiler<T>::Compiler() noexcept :
		cpu_()
	{
		Logger::init();
	}

	template<typename T>
	inline Compiler<T>::Compiler(const Compiler &crComp) noexcept:
		cpu_(crComp.cpu_)
	{ }

	template<typename T>
	inline Compiler<T>::Compiler(Compiler &&rrComp) noexcept :
		cpu_(std::move(rrComp.cpu_))
	{ }

	template<typename T>
	inline Compiler<T>::~Compiler()
	{
		Logger::close();
	}

	template<typename T>
	T Compiler<T>::getValue(std::string_view str) const
	{	
		if (str.front() == '[') str.remove_prefix(1);
		
		std::stringstream sstr(str.data());
		T val = T();
		sstr >> val;

		return val;
	}

	template<typename T>
	REG Compiler<T>::makeReg(std::string_view str) const
	{
		if      (str == "ax" || str == "[ax]") return REG::AX;
		else if (str == "bx" || str == "[bx]") return REG::BX;
		else if (str == "cx" || str == "[cx]") return REG::CX;
		else if (str == "dx" || str == "[dx]") return REG::DX;
		else if (str == "ex" || str == "[ex]") return REG::EX;
		else if (str == "sp" || str == "[sp]") return REG::SP;

		else                                   return REG::NUM;
	}

	template<typename T>
	inline bool Compiler<T>::isReg(std::string_view val) const
	{
		return (makeReg(val) != REG::NUM);
	}

	template<typename T>
	inline Compiler<T> &Compiler<T>::operator=(const Compiler &crComp) noexcept
	{
		if (this != &crComp) cpu_ = crComp.cpu_;

		return (*this);
	}

	template<typename T>
	inline Compiler<T> &Compiler<T>::operator=(Compiler &&rrComp) noexcept
	{
		assert(this != &rrComp);

		cpu_ = std::move(rrComp.cpu_);

		return (*this);
	}

#pragma region Functions Compiler<>::toSomeFile

	template<typename T>
	bool Compiler<T>::text2com(const std::string &crFilename) const
	{
		std::ifstream input(crFilename + ".txt");
		if (!input.is_open())
		{
			NDebugger::Error("Cannot open file: " + crFilename);

			return false;
		}

		std::ofstream output(crFilename + "Com.txt");
		if (!output.is_open())
		{
			NDebugger::Error("Cannot open file: " + crFilename + "Com");

			input.close();

			return false;
		}

		while (!input.eof())
		{
			char tmp[MAX_LINE_LENGTH] = { };
			input.getline(tmp, MAX_LINE_LENGTH, '\n'); // Get the line from input file

			Operation op(std::move(ParseCode(tmp))); // Parse the line
			if (!op.cmd.length()) // Skip the empty line
				continue;  

			else if (op.cmd[0] == ':' || op.cmd[op.cmd.length() - 1] == ':') // Write the label or signature of the function
				output << op.cmd;

			else if (auto it = std::find_if(std::begin(cgTable), std::end(cgTable), [&] (const Command &com) -> bool { return (com.name == op.cmd); }); it != std::end(cgTable)) // Search the cmd in cgTable
			{
				output << static_cast<unsigned>(it->number); // Write command number

				if (char num = it->numOfArgs; num != '0')
					for (auto i = '0'; i < num; ++i)
						output << " " << op.args[i - '0']; // Write args
			}
			
			else // Unknown command
			{
				NDebugger::Error("Unknown command: " + std::string(op.cmd));

				output.close();
				input.close();

				return false;
			}

			output << std::endl;
		}

		output.close();
		input.close();

		return true;
	}

	template<typename T>
	bool Compiler<T>::text2bin(const std::string &crFilename) const
	{
		std::ifstream input(crFilename + ".txt");
		if (!input.is_open())
		{
			NDebugger::Error("Cannot open file: " + crFilename);

			return false;
		}

		std::ofstream output(crFilename + "BinText.txt", std::ios::binary);
		if (!output.is_open())
		{
			NDebugger::Error("Cannot open file: " + crFilename + "BinText");

			input.close();

			return false;
		}

#define WRITE_STRING(str) Wrap4BinaryIO<std::string>(str)
#define ARG(index)        WRITE_STRING(op.args[index])

		while (!input.eof())
		{
			char tmp[MAX_LINE_LENGTH] = { };
			input.getline(tmp, MAX_LINE_LENGTH, '\n'); // Get the line from input file

			Operation op(std::move(ParseCode(tmp))); // Parse the line
			if (!op.cmd.length()) // Skip the empty line
				continue;		

			else if (op.cmd[0] == ':' || op.cmd[op.cmd.length() - 1] == ':') // Write the label or signature of the function
				output << WRITE_STRING(op.cmd);

			else if (auto it = std::find_if(std::begin(cgTable), std::end(cgTable), [&](const Command &com) -> bool { return (com.name == op.cmd); }); it != std::end(cgTable)) // Search the cmd in cgTable
			{
				output << WRITE_STRING(op.cmd); // Write command name

				if (char num = it->numOfArgs; num != '0')
					for (auto i = '0'; i < num; ++i)
						output << ARG(i - '0'); // Write args
			}

			else // Unknown command
			{
				NDebugger::Error("Unknown command: " + op.cmd);

				output.close();
				input.close();

				return false;
			}
		}

#undef ARG
#undef WRITE_STRING

		output.close();
		input.close();

		return true;
	}

	template<typename T>
	bool Compiler<T>::com2bin(const std::string &crFilename) const
	{
		std::ifstream input(crFilename + "Com.txt");
		if (!input.is_open())
		{
			NDebugger::Error("Cannot open file: " + crFilename + "Com");

			return false;
		}

		std::ofstream output(crFilename + "BinCom.txt", std::ios::binary);
		if (!output.is_open())
		{
			NDebugger::Error("Cannot open file: " + crFilename + "BinCom");

			input.close();

			return false;
		}

#define WRITE_STRING(str) Wrap4BinaryIO<std::string>(str)
#define ARG(index)        WRITE_STRING(op.args[index])

		while (!input.eof())
		{
			char tmp[MAX_LINE_LENGTH] = { };
			input.getline(tmp, MAX_LINE_LENGTH, '\n'); // Get the line from input file

			Operation op(std::move(ParseCode(tmp))); // Parse the line
			if (!op.cmd.length()) // Skip the empty line
				continue;

			else if (op.cmd[0] == ':' || op.cmd[op.cmd.length() - 1] == ':') // Write the label or signature of the function
			{
				output << WRITE_STRING(op.cmd);

				continue;
			}

			Commands cmd = static_cast<Commands>(std::stoi(op.cmd));
			if (auto it = std::find_if(std::begin(cgTable), std::end(cgTable), [&](const Command &com) -> bool { return (com.number == cmd); }); it != std::end(cgTable)) // Search the cmd in cgTable
			{
				output << WRITE_STRING(op.cmd); // Write command number

				if (char num = it->numOfArgs; num != '0')
					for (auto i = '0'; i < num; ++i)
						output << ARG(i - '0'); // Write args
			}

			else // Unknown command
			{
				NDebugger::Error("Unknown command: " + op.cmd);

				input.close();
				output.close();

				return false;
			}
		}

#undef ARG
#undef WRITE_STRING

		output.close();
		input.close();

		return true;
	}

#pragma endregion

#pragma region Functions Compiler<>::fromSomeFile

	template<typename T>
	bool Compiler<T>::fromTextFile(const std::string &crFilename)
	{
		std::ifstream file(crFilename + ".txt");
		if (!file.is_open())
		{
			NDebugger::Error("Cannot open file: " + crFilename);

			return false;
		}

		while (!file.eof())
		{
			char tmp[MAX_LINE_LENGTH] = { };
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
				bool isArg0Reg = isReg(op.args[0]),
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

				return false;
			}
		}

		file.close();

		return true;
	}

	template<typename T>
	bool Compiler<T>::fromComFile(const std::string &crFilename)
	{
		std::ifstream file(crFilename + "Com.txt");
		if (!file.is_open())
		{
			NDebugger::Error("Cannot open file: " + crFilename + "Com");

			return false;
		}

		while (!file.eof())
		{
			char tmp[MAX_LINE_LENGTH] = { };
			file.getline(tmp, MAX_LINE_LENGTH, '\n'); // Get the line from file

			Operation op(std::move(ParseCode(tmp))); // Parse the line
			if (!op.cmd.length() || op.cmd[0] == ':' || op.cmd == "null") continue; // Skip the label or zero-line

			else if (op.cmd[op.cmd.length() - 1] == ':') // Skip the function
			{
				do
				{
					file.getline(tmp, MAX_LINE_LENGTH, '\n');
					op = std::move(ParseCode(tmp));
				} while (std::stoi(op.cmd) != static_cast<int>(Commands::ret));

				continue;
			}

			int command = std::stoi(op.cmd);
			if (command == static_cast<int>(Commands::push))
			{
				auto val = getValue(op.args[0]);
				auto reg = makeReg(op.args[0]);

				if (op.args[0][0] == '[' && reg != REG::NUM) cpu_.push(reg, CPU<>::MemoryStorage::RAM);
				else if (op.args[0][0] == '[' && reg == REG::NUM) cpu_.push(val, CPU<>::MemoryStorage::RAM);
				else if (reg != REG::NUM) cpu_.push(reg, CPU<>::MemoryStorage::STACK);
				else		                                      cpu_.push(val, CPU<>::MemoryStorage::STACK);
			}
			else if (command == static_cast<int>(Commands::pop))
			{
				if (op.args[0][0] == '[') cpu_.pop(CPU<>::MemoryStorage::RAM);
				else                      cpu_.pop(CPU<>::MemoryStorage::STACK);
			}

			else if (command == static_cast<int>(Commands::add))  cpu_.add();
			else if (command == static_cast<int>(Commands::sub))  cpu_.sub();
			else if (command == static_cast<int>(Commands::div))  cpu_.div();
			else if (command == static_cast<int>(Commands::dup))  cpu_.dup();
			else if (command == static_cast<int>(Commands::sin))  cpu_.sin();
			else if (command == static_cast<int>(Commands::cos))  cpu_.cos();
			else if (command == static_cast<int>(Commands::sqrt)) cpu_.sqrt();

			else if (command == static_cast<int>(Commands::dump)) cpu_.dump();

			else if (command == static_cast<int>(Commands::cmp))  cpu_.push(getValue(op.args[0]), CPU<>::MemoryStorage::STACK),
				cpu_.push(getValue(op.args[1]), CPU<>::MemoryStorage::STACK);
			else if (command == static_cast<int>(Commands::jump)) Move2Label(file, ':' + op.args[0]);

			else if (command == static_cast<int>(Commands::je)) { auto pair = cpu_.getPair(); if (pair.first == pair.second) Move2Label(file, ':' + op.args[0]); }
			else if (command == static_cast<int>(Commands::jne)) { auto pair = cpu_.getPair(); if (pair.first != pair.second) Move2Label(file, ':' + op.args[0]); }
			else if (command == static_cast<int>(Commands::ja)) { auto pair = cpu_.getPair(); if (pair.first > pair.second) Move2Label(file, ':' + op.args[0]); }
			else if (command == static_cast<int>(Commands::jae)) { auto pair = cpu_.getPair(); if (pair.first >= pair.second) Move2Label(file, ':' + op.args[0]); }
			else if (command == static_cast<int>(Commands::jb)) { auto pair = cpu_.getPair(); if (pair.first < pair.second) Move2Label(file, ':' + op.args[0]); }
			else if (command == static_cast<int>(Commands::jbe)) { auto pair = cpu_.getPair(); if (pair.first <= pair.second) Move2Label(file, ':' + op.args[0]); }

			else if (command == static_cast<int>(Commands::move))
			{
				bool isArg0Reg = isReg(op.args[0]),
					isArg1Reg = isReg(op.args[1]);

				if (isArg0Reg &&  isArg1Reg) cpu_.move(makeReg(op.args[0]), makeReg(op.args[1]));
				else if (!isArg0Reg &&  isArg1Reg) cpu_.move(getValue(op.args[0]), makeReg(op.args[1]));
			}

			else if (command == static_cast<int>(Commands::call))
			{
				cpu_.push(file.tellg());
				Move2Label(file, op.args[0] + ':');
			}
			else if (command == static_cast<int>(Commands::ret))
			{
				file.seekg(cpu_.top());
				cpu_.pop(CPU<>::MemoryStorage::STACK_FUNC_RET_ADDR);
			}

			else if (command == static_cast<int>(Commands::end)) break;

			else
			{
				NDebugger::Error("Unknown command: " + command);

				file.close();

				return false;
			}
		}

		file.close();

		return true;
	}

	template<typename T>
	bool Compiler<T>::fromBinTextFile(const std::string &crFilename)
	{
		std::ifstream file(crFilename + "BinText.txt", std::ios::binary);
		if (!file.is_open())
		{
			NDebugger::Error("Cannot open file: " + crFilename + "BinText");

			return false;
		}

#define COMMAND command.operator const std::string&()

		bool skipCommand = false;
		while (!file.eof())
		{
			Wrap4BinaryIO<std::string> command;
			file >> command;

			std::cout << command << std::endl;
			
			if (!COMMAND.length() || COMMAND[0] == ':') continue; // Skip the label

			else if (COMMAND[COMMAND.length() - 1] == ':') skipCommand = true; // Skip the function

			else if (COMMAND == "push")
			{
				file >> command;

				if (skipCommand) continue;

				auto val = getValue(command.operator const std::string&());
				auto reg = makeReg(command.operator const std::string&());

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
				auto pushVal = [&]() mutable
				{
					file >> command;
					if (!skipCommand)
					{
						auto reg = makeReg(command.operator const std::string&());

						if (reg != REG::NUM) cpu_.push(reg, CPU<>::MemoryStorage::STACK);
						else                 cpu_.push(getValue(command.operator const std::string&()), CPU<>::MemoryStorage::STACK);
					}
				};

				pushVal();
				pushVal();
			}
			else if (COMMAND == "jump") { file >> command; if (skipCommand) continue; Move2LabelBin(file, command.operator const std::string&()); }

			else if (COMMAND == "je")  { file >> command; if (skipCommand) continue; auto pair = cpu_.getPair(); if (pair.first == pair.second) Move2LabelBin(file, command.operator const std::string&()); }
			else if (COMMAND == "jne") { file >> command; if (skipCommand) continue; auto pair = cpu_.getPair(); if (pair.first != pair.second) Move2LabelBin(file, command.operator const std::string&()); }
			else if (COMMAND == "ja")  { file >> command; if (skipCommand) continue; auto pair = cpu_.getPair(); if (pair.first  > pair.second) Move2LabelBin(file, command.operator const std::string&()); }
			else if (COMMAND == "jae") { file >> command; if (skipCommand) continue; auto pair = cpu_.getPair(); if (pair.first >= pair.second) Move2LabelBin(file, command.operator const std::string&()); }
			else if (COMMAND == "jb")  { file >> command; if (skipCommand) continue; auto pair = cpu_.getPair(); if (pair.first  < pair.second) Move2LabelBin(file, command.operator const std::string&()); }
			else if (COMMAND == "jbe") { file >> command; if (skipCommand) continue; auto pair = cpu_.getPair(); if (pair.first <= pair.second) Move2LabelBin(file, command.operator const std::string&()); }

			else if (COMMAND == "move")
			{
				Wrap4BinaryIO<std::string> arg0,
					                       arg1;
				file >> arg0 >> arg1;

				if (skipCommand) continue;

				bool isArg0Reg = isReg(arg0.operator const std::string&()),
					 isArg1Reg = isReg(arg1.operator const std::string&());

				if (isArg0Reg &&  isArg1Reg) cpu_.move(makeReg(arg0.operator const std::string&()), makeReg(arg1.operator const std::string&()));
				else if (!isArg0Reg &&  isArg1Reg) cpu_.move(getValue(arg0.operator const std::string&()), makeReg(arg1.operator const std::string&()));
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
					skipCommand = false;

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

				return false;
			}
		}

#undef COMMAND

		file.close();

		return true;
	}

	template<typename T>
	bool Compiler<T>::fromBinComFile(const std::string &crFilename)
	{
		std::ifstream file(crFilename + "BinCom.txt", std::ios::binary);
		if (!file.is_open())
		{
			NDebugger::Error("Cannot open file: " + crFilename + "BinCom");

			return false;
		}

#define COMMAND command.operator const std::string&()

		bool skipCommand = false;
		while (!file.eof())
		{
			Wrap4BinaryIO<std::string> command;
			file >> command;
			
			std::cout << command << std::endl;

			if (!COMMAND.length() || COMMAND[0] == ':') continue; // Skip the label

			else if (COMMAND[COMMAND.length() - 1] == ':') { skipCommand = true; continue; }

			int comInNum = std::stoi(COMMAND);
			if      (comInNum == static_cast<int>(Commands::push))
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
			else if (comInNum == static_cast<int>(Commands::pop))
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

			else if (comInNum == static_cast<int>(Commands::add))  { if (!skipCommand) cpu_.add(); }
			else if (comInNum == static_cast<int>(Commands::sub))  { if (!skipCommand) cpu_.sub(); }
			else if (comInNum == static_cast<int>(Commands::mul))  { if (!skipCommand) cpu_.mul(); }
			else if (comInNum == static_cast<int>(Commands::div))  { if (!skipCommand) cpu_.div(); }
			else if (comInNum == static_cast<int>(Commands::dup))  { if (!skipCommand) cpu_.dup(); }
			else if (comInNum == static_cast<int>(Commands::sin))  { if (!skipCommand) cpu_.sin(); }
			else if (comInNum == static_cast<int>(Commands::cos))  { if (!skipCommand) cpu_.cos(); }
			else if (comInNum == static_cast<int>(Commands::sqrt)) { if (!skipCommand) cpu_.sqrt(); }

			else if (comInNum == static_cast<int>(Commands::dump)) { if (!skipCommand) cpu_.dump(); }

			else if (comInNum == static_cast<int>(Commands::cmp))
			{
				auto pushVal = [&]() mutable
				{
					file >> command;
					if (!skipCommand)
					{
						auto reg = makeReg(command.operator const std::string&());

						if (reg != REG::NUM) cpu_.push(reg, CPU<>::MemoryStorage::STACK);
						else                 cpu_.push(getValue(command.operator const std::string&()), CPU<>::MemoryStorage::STACK);
					}
				};

				pushVal();
				pushVal();
			}
			else if (comInNum == static_cast<int>(Commands::jump)) { file >> command; if (skipCommand) continue; Move2LabelBin(file, command.operator const std::string&()); }

			else if (comInNum == static_cast<int>(Commands::je))  { file >> command; if (skipCommand) continue; auto pair = cpu_.getPair(); if (pair.first == pair.second) Move2LabelBin(file, command.operator const std::string&()); }
			else if (comInNum == static_cast<int>(Commands::jne)) { file >> command; if (skipCommand) continue; auto pair = cpu_.getPair(); if (pair.first != pair.second) Move2LabelBin(file, command.operator const std::string&()); }
			else if (comInNum == static_cast<int>(Commands::ja))  { file >> command; if (skipCommand) continue; auto pair = cpu_.getPair(); if (pair.first  > pair.second) Move2LabelBin(file, command.operator const std::string&()); }
			else if (comInNum == static_cast<int>(Commands::jae)) { file >> command; if (skipCommand) continue; auto pair = cpu_.getPair(); if (pair.first >= pair.second) Move2LabelBin(file, command.operator const std::string&()); }
			else if (comInNum == static_cast<int>(Commands::jb))  { file >> command; if (skipCommand) continue; auto pair = cpu_.getPair(); if (pair.first  < pair.second) Move2LabelBin(file, command.operator const std::string&()); }
			else if (comInNum == static_cast<int>(Commands::jbe)) { file >> command; if (skipCommand) continue; auto pair = cpu_.getPair(); if (pair.first <= pair.second) Move2LabelBin(file, command.operator const std::string&()); }

			else if (comInNum == static_cast<int>(Commands::move))
			{
				Wrap4BinaryIO<std::string> arg0,
					                       arg1;
				file >> arg0 >> arg1;

				if (skipCommand) continue;

				bool isArg0Reg = isReg(arg0.operator const std::string&()),
					 isArg1Reg = isReg(arg1.operator const std::string&());

				if      ( isArg0Reg &&  isArg1Reg) cpu_.move(makeReg(arg0.operator const std::string&()), makeReg(arg1.operator const std::string&()));
				else if (!isArg0Reg &&  isArg1Reg) cpu_.move(getValue(arg0.operator const std::string&()), makeReg(arg1.operator const std::string&()));
			}

			else if (comInNum == static_cast<int>(Commands::call))
			{
				file >> command;
	
				if (skipCommand) continue;

				cpu_.push(file.tellg());
				Move2LabelBin(file, COMMAND + ':');
			}
			else if (comInNum == static_cast<int>(Commands::ret))
			{
				if (skipCommand)
				{
					skipCommand = false;

					continue;
				}

				file.seekg(cpu_.top());
				cpu_.pop(CPU<>::MemoryStorage::STACK_FUNC_RET_ADDR);
			}

			else if (comInNum == static_cast<int>(Commands::end)) { if (!skipCommand) break; }

			else
			{
				NDebugger::Error("Unknown command: " + comInNum);

				file.close();

				return false;
			}
		}

#undef COMMAND

		file.close();

		return true;
	}

#pragma endregion
} // NCompiler

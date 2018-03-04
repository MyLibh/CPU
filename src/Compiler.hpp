#pragma once

#include <fstream>  // std::ifstream, std::ofstream
#include <map>      // std::map
#include <iterator> // std::istream_iterator
#include <vector>   // std::vector
#include <filesystem> // std::path

#include "Parser.hpp"
#include "Wrap4BinaryIO.hpp"
#include "CPUCommands.hpp"

namespace NCompiler
{

//====================================================================================================================================
//==============================================================USINGS================================================================
//====================================================================================================================================

#pragma region USINGS

	using namespace NParser;
	using namespace NCpu;
	using namespace NCpu::Commands;

#pragma endregion

//====================================================================================================================================
//==============================================================CLASSES===============================================================
//====================================================================================================================================

	template<typename T = int>
	class Compiler final
	{
		std::vector<Operation> load(std::experimental::filesystem::path);

	public:
		explicit Compiler()       = default;
		Compiler(const Compiler&) = default;
		Compiler(Compiler&&)      = default;
		~Compiler()               = default;        

		Compiler<T> &operator=(const Compiler&) noexcept;
		Compiler<T> &operator=(Compiler&&)      noexcept;

		bool text2com(std::experimental::filesystem::path) const;
		bool text2bin(std::experimental::filesystem::path) const;
		bool com2bin(std::experimental::filesystem::path)  const;

		bool fromTextFile(std::experimental::filesystem::path);
		bool fromComFile(std::experimental::filesystem::path);
		bool fromBinTextFile(std::experimental::filesystem::path);
		bool fromBinComFile(std::experimental::filesystem::path);

	private:
		std::map<std::string, size_t> labels_;
		CPU<T>                        cpu_;
	};

//====================================================================================================================================
//========================================================FUNCTION_DECLARATION========================================================
//====================================================================================================================================

	inline std::string wstr2str(std::wstring_view);

//====================================================================================================================================
//=========================================================METHOD_DEFINITION==========================================================
//====================================================================================================================================

#pragma region METHOD_DEFINITION

	template<typename T>
	std::vector<Operation> Compiler<T>::load(std::experimental::filesystem::path path)
	{
		std::ifstream file(path.generic_string() + ".txt");
		if (!file.is_open())
		{
			NDebugger::Error("Cannot open file: " + path.generic_string());
	
			return std::vector<Operation>();
		}

		std::vector<Operation> programm;
		while (!file.eof())
		{
			char tmp[MAX_LINE_LENGTH] = {};
			file.getline(tmp, MAX_LINE_LENGTH, '\n');

			if (auto op = std::move(ParseCode(tmp)); op.cmd.length())
			{
				if (op.cmd.front() == ':')
					labels_[std::string(op.cmd.begin() + 1, op.cmd.end())] = programm.size();
				else if (op.cmd.back() == ':')
					labels_[std::string(op.cmd.begin(), op.cmd.end() - 1)] = programm.size();

				programm.push_back(op);
			}
		}

		file.close();

		return programm;
	}

	template<typename T>
	inline Compiler<T> &Compiler<T>::operator=(const Compiler &crComp) noexcept
	{
		if (this != &crComp) 
			cpu_ = crComp.cpu_;

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
	bool Compiler<T>::text2com(std::experimental::filesystem::path path) const
	{
		std::ifstream input(path.generic_string() + ".txt");
		if (!input.is_open())
		{
			NDebugger::Error("Cannot open file: " + path.generic_string());

			return false;
		}

		std::ofstream output(path.generic_string() + "Com.txt");
		if (!output.is_open())
		{
			NDebugger::Error("Cannot open file: " + path.generic_string() + "Com");

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

			else if (auto it = std::find_if(CPU_COMMANDS<T>::cbegin(), CPU_COMMANDS<T>::cend(), [&] (auto &&com) -> bool { return (com.name == op.cmd); }); it != CPU_COMMANDS<T>::cend()) // Search the cmd in cgTable
			{
				output << static_cast<unsigned>(it->number); // Write command number

				if (char num = it->numOfArgs; num != '0')
					for (auto i = '0'; i < num; ++i)
						output << " " << op.args[i - '0']; // Write args
			}
			
			else // Unknown command
			{
				NDebugger::Error("Unknown command: " + op.cmd);

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
	bool Compiler<T>::text2bin(std::experimental::filesystem::path path) const
	{
		std::ifstream input(path.generic_string() + ".txt");
		if (!input.is_open())
		{
			NDebugger::Error("Cannot open file: " + path.generic_string());

			return false;
		}

		std::ofstream output(path.generic_string() + "BinText.txt", std::ios::binary);
		if (!output.is_open())
		{
			NDebugger::Error("Cannot open file: " + path.generic_string() + "BinText");

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

			else if (auto it = std::find_if(CPU_COMMANDS<T>::cbegin(), CPU_COMMANDS<T>::cend(), [&](auto &&com) -> bool { return (com.name == op.cmd); }); it != CPU_COMMANDS<T>::cend()) // Search the cmd in cgTable
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
	bool Compiler<T>::com2bin(std::experimental::filesystem::path path) const
	{
		std::ifstream input(path.generic_string() + "Com.txt");
		if (!input.is_open())
		{
			NDebugger::Error("Cannot open file: " + path.generic_string() + "Com");

			return false;
		}

		std::ofstream output(path.generic_string() + "BinCom.txt", std::ios::binary);
		if (!output.is_open())
		{
			NDebugger::Error("Cannot open file: " + path.generic_string() + "BinCom");

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
			if (auto it = std::find_if(CPU_COMMANDS<T>::cbegin(), CPU_COMMANDS<T>::cend(), [&](const Command &com) -> bool { return (com.number == cmd); }); it != CPU_COMMANDS<T>::cend()) // Search the cmd in cgTable
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
	bool Compiler<T>::fromTextFile(std::experimental::filesystem::path path)
	{
		auto programm = std::move(load(path));
		for(auto op = programm.begin(); op != programm.end(); op++)
		{
			if (op->cmd.front() == ':')  // Skip the label
				continue;
			else if (op->cmd.back() == ':') // Skip the function
			{
				do
				{
					op++;
				} while (op->cmd != "ret" && op != programm.cend());

				continue;
			}
			else if (op->cmd == "end")
				break;
	
			if (auto it = std::find_if(CPU_COMMANDS<T>::cbegin(), CPU_COMMANDS<T>::cend(), [&](auto &&com) -> bool { return (com.name == op->cmd); }); it != CPU_COMMANDS<T>::cend())
			{
				short code = it->func(cpu_, op->args);
				switch(code)
				{
				case 0:
					break;

				case 1:
					op = programm.begin() + labels_[op->args[0]];
					break;

				case 2:
					cpu_.push(std::distance(programm.begin(), op));
					op = programm.begin() + labels_[op->args[0]];
					break;
					
				default:
					op = programm.begin() + code;
					break;
				}

			}
			else
			{
				NDebugger::Error("Unknown command: " + op->cmd);

				return false;
			}
		}

		return true;
	}

	template<typename T>
	bool Compiler<T>::fromComFile(std::experimental::filesystem::path path)
	{
		auto programm = std::move(load(path));
		for each (auto x in labels_)
		{
			std::cout << x.first << " " << x.second << std::endl;
		}
		for (auto op = programm.begin(); op != programm.end(); op++)
		{
			op->dump();
			if (op->cmd.front() == ':')  // Skip the label
				continue;
			else if (op->cmd.back() == ':') // Skip the function
			{
				do
				{
					op++;
				} while (std::stoi(op->cmd) == static_cast<unsigned>(NCpu::Commands::Commands::ret) && op != programm.cend());

				continue;
			}
			else if (std::stoi(op->cmd) == static_cast<unsigned>(NCpu::Commands::Commands::end))
				break;

			unsigned cmd = std::stoi(op->cmd);
			if (auto it = std::find_if(CPU_COMMANDS<T>::cbegin(), CPU_COMMANDS<T>::cend(), [&](auto &&com) -> bool { return (static_cast<unsigned>(com.number) == cmd); }); it != CPU_COMMANDS<T>::cend())
			{
				short code = it->func(cpu_, op->args);
				switch (code)
				{
				case 0:
					break;

				case 1:
					op = programm.begin() + labels_[op->args[0]];
					break; 

				case 2:
					cpu_.push(std::distance(programm.begin(), op));
					op = programm.begin() + labels_[op->args[0]];
					break;
			
				default:
					op = programm.begin() + code;
					break;
				
				}

			}
			else
			{
				NDebugger::Error("Unknown command: " + op->cmd, std::cerr);

				return false;
			}
		}

		return true;
	}

	template<typename T>
	bool Compiler<T>::fromBinTextFile(std::experimental::filesystem::path path)
	{
		std::ifstream file(path.generic_string() + "BinText.txt", std::ios::binary);
		if (!file.is_open())
		{
			NDebugger::Error("Cannot open file: " + path.generic_string() + "BinText");

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
	bool Compiler<T>::fromBinComFile(std::experimental::filesystem::path path)
	{
		std::ifstream file(path.generic_string() + "BinCom.txt", std::ios::binary);
		if (!file.is_open())
		{
			NDebugger::Error("Cannot open file: " + path.generic_string() + "BinCom");

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

#pragma endregion

//====================================================================================================================================
//========================================================FUNCTION_DEFINITION=========================================================
//====================================================================================================================================

	inline std::string wstr2str(std::wstring_view str)
	{ // Deprecated in C++17
		return std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>>().to_bytes(str.data());
	}

} // NCompiler

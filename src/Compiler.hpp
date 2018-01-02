#pragma once

#include <fstream>
#include <sstream>

#include "Parser.hpp"
#include "CPU.hpp"

namespace NCompiler
{
#pragma region Class Wrap2BinaryIO
	
	template<typename T>
	class Wrap2BinaryIO
	{
	public:
		Wrap2BinaryIO();
		Wrap2BinaryIO(CONST T&);
		~Wrap2BinaryIO();

		operator T&();
		operator CONST T&() const;

	private:
		T val_;
	};

	//====================================================================================================================================

	template<typename T>
	std::istream &operator>>(std::istream&, Wrap2BinaryIO<T>&);

	template<typename T>
	std::ostream &operator<<(std::ostream&, Wrap2BinaryIO<T>&);

	//====================================================================================================================================

	template<typename T>
	Wrap2BinaryIO<T>::Wrap2BinaryIO() :
		val_()
	{ }

	template<typename T>
	Wrap2BinaryIO<T>::Wrap2BinaryIO(CONST T &val) :
		val_(val)
	{ }

	template<typename T>
	Wrap2BinaryIO<T>::~Wrap2BinaryIO()
	{ }

	template<typename T>
	Wrap2BinaryIO<T>::operator T&()
	{
		return val_;
	}

	template<typename T>
	Wrap2BinaryIO<T>::operator CONST T&() const
	{
		return val_;
	}

	//====================================================================================================================================

	template<typename T>
	std::istream &operator>>(std::istream &rIstr, Wrap2BinaryIO<T> &rVal)
	{
		rIstr.read(reinterpret_cast<CHAR*>(&static_cast<T&>(rVal)), sizeof(T));

		return rIstr;
	}

	template<typename T>
	std::ostream &operator<<(std::ostream &rOstr, Wrap2BinaryIO<T> &rVal)
	{
		rOstr.write(reinterpret_cast<CONST CHAR*>(&static_cast<T&>(rVal)), sizeof(T));

		return rOstr;
	}
#pragma endregion

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
		enum commands_ : UNSIGNED
		{
			undefined = 0,

			push = 1,
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

			je,
			jne,
			ja,
			jae,
			jb,
			jbe,

			move,

			end
		};

		explicit Compiler()       _NOEXCEPT;
		Compiler(CONST Compiler&);
		Compiler(Compiler&&)      _NOEXCEPT;
		~Compiler();

		Compiler<T> &operator=(CONST Compiler&);
		Compiler<T> &operator=(Compiler&&);

		BOOL toComFile(CRSTRING) const;
		BOOL toBinFile(CRSTRING) const;

		BOOL fromTextFile(CRSTRING);
		BOOL fromComFile(CRSTRING);
		BOOL fromBinFile(CRSTRING);
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

		return (val ? val : undefined);
	}

	template<typename T>
	REG Compiler<T>::makeReg(CRSTRING str) const
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
	BOOL Compiler<T>::toComFile(CRSTRING filename) const
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
			NDebugger::Error("Cannot open file: " + outputFile);

			input.close();

			return FALSE;
		}

		while (!input.eof())
		{
			CHAR tmp[MAX_LINE_LENGTH] = {};
			input.getline(tmp, MAX_LINE_LENGTH, '\n');

			Operation *pOp = ParseCode(tmp);
			if (pOp)
			{
				if      (pOp->cmd == "push") output << commands_::push << " " << pOp->args[0];
				else if (pOp->cmd == "pop")  output << commands_::pop  << " " << pOp->args[0];

				else if (pOp->cmd == "add")	 output << commands_::add;
				else if (pOp->cmd == "sub")  output << commands_::sub;
				else if (pOp->cmd == "mul")	 output << commands_::mul;
				else if (pOp->cmd == "div")	 output << commands_::div;
				else if (pOp->cmd == "dup")	 output << commands_::dup;
				else if (pOp->cmd == "sin")  output << commands_::sin;
				else if (pOp->cmd == "cos")  output << commands_::cos;
				else if (pOp->cmd == "sqrt") output << commands_::sqrt;

				else if (pOp->cmd == "dump") output << commands_::dump;

				else if (pOp->cmd == "cmp")  output << commands_::cmp  << " " << pOp->args[0] << " " << pOp->args[1];
				else if (pOp->cmd == "jump") output << commands_::jump << " " << pOp->args[0];

				else if (pOp->cmd == "je")	 output << commands_::je  << " " << pOp->args[0];
				else if (pOp->cmd == "jne")  output << commands_::jne << " " << pOp->args[0];
				else if (pOp->cmd == "ja")	 output << commands_::ja  << " " << pOp->args[0];
				else if (pOp->cmd == "jae")	 output << commands_::jae << " " << pOp->args[0];
				else if (pOp->cmd == "jb")	 output << commands_::jb  << " " << pOp->args[0];
				else if (pOp->cmd == "jbe")  output << commands_::jbe << " " << pOp->args[0];

				else if (pOp->cmd == "move") output << commands_::move << " " << pOp->args[0] << " " << pOp->args[1];

				else if (pOp->cmd == "end")  output << commands_::end;

				else if (pOp->cmd[0] == ':' || !pOp->cmd.length()) output << pOp->cmd;

				else
				{
					NDebugger::Error("Unknown command: " + std::string(tmp));

					delete pOp;

					output.close();
					input.close();

					return FALSE;
				}

				output << std::endl;
			}
			else break;

			delete pOp;
		}

		output.close();
		input.close();

		return TRUE;
	}

	template<typename T>
	BOOL Compiler<T>::toBinFile(CRSTRING filename) const
	{
		std::ifstream input(filename + ".txt");
		if (!input.is_open())
		{
			NDebugger::Error("Cannot open file: " + filename);

			return FALSE;
		}

		std::string outputFile = filename + "Bin" + ".txt";
		std::ofstream output(outputFile, std::ios::binary);
		if (!output.is_open())
		{
			NDebugger::Error("Cannot open file: " + outputFile);

			input.close();

			return FALSE;
		}

		while (!input.eof())
		{
			CHAR tmp[MAX_LINE_LENGTH] = {};
			input.getline(tmp, MAX_LINE_LENGTH, '\n');

			Operation *pOp = ParseCode(tmp);
			if (pOp)
			{
				if(!pOp->cmd.length()) continue;

#define COMMAND(command) Wrap2BinaryIO<WORD>(command)
#define ARG(index)       Wrap2BinaryIO<std::string>(pOp->args[index])
				
#define SPACE            Wrap2BinaryIO<std::string>(" ")

				if      (pOp->cmd == "push") output << COMMAND(commands_::push) << SPACE << ARG(0); 
				else if (pOp->cmd == "pop")  output << COMMAND(commands_::pop)  << SPACE << ARG(0);

				else if (pOp->cmd == "add")	 output << COMMAND(commands_::add);
				else if (pOp->cmd == "sub")  output << COMMAND(commands_::sub);
				else if (pOp->cmd == "mul")	 output << COMMAND(commands_::mul);
				else if (pOp->cmd == "div")	 output << COMMAND(commands_::div);
				else if (pOp->cmd == "dup")	 output << COMMAND(commands_::dup);
				else if (pOp->cmd == "sin")  output << COMMAND(commands_::sin);
				else if (pOp->cmd == "cos")  output << COMMAND(commands_::cos);
				else if (pOp->cmd == "sqrt") output << COMMAND(commands_::sqrt);

				else if (pOp->cmd == "dump") output << COMMAND(commands_::dump);

				else if (pOp->cmd == "cmp")  output << COMMAND(commands_::cmp)  << SPACE << ARG(0) << SPACE << ARG(1);
				else if (pOp->cmd == "jump") output << COMMAND(commands_::jump) << SPACE << ARG(0);

				else if (pOp->cmd == "je")	 output << COMMAND(commands_::je)  << SPACE << ARG(0);
				else if (pOp->cmd == "jne")  output << COMMAND(commands_::jne) << SPACE << ARG(0);
				else if (pOp->cmd == "ja")	 output << COMMAND(commands_::ja)  << SPACE << ARG(0);
				else if (pOp->cmd == "jae")	 output << COMMAND(commands_::jae) << SPACE << ARG(0);
				else if (pOp->cmd == "jb")	 output << COMMAND(commands_::jb)  << SPACE << ARG(0);
				else if (pOp->cmd == "jbe")  output << COMMAND(commands_::jbe) << SPACE << ARG(0);

				else if (pOp->cmd == "move") output << COMMAND(commands_::move) << SPACE << ARG(0) << SPACE << ARG(1);

				else if (pOp->cmd == "end")  output << COMMAND(commands_::end);

				else if (pOp->cmd[0] == ':' || !pOp->cmd.length()) output << Wrap2BinaryIO<std::string>(pOp->cmd);

				else
				{
					NDebugger::Error("Unknown command: " + std::string(tmp));

					delete pOp;

					output.close();
					input.close();

					return FALSE;
				}

			//output << SPACE;

#undef SPACE

#undef ARG
#undef COMMAND
			}
			else break;
			
			delete pOp;
		}

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

			Operation *pOp = ParseCode(tmp);
			if (pOp)
			{
				if (pOp->cmd == "push")
				{
					auto val = getValue(pOp->args[0]);
					auto reg = makeReg(pOp->args[0]);

					if      (pOp->args[0][0] == '[' && reg != REG::NUM) cpu_.push(reg, CPU<>::MemoryStorage::RAM);
					else if (pOp->args[0][0] == '[' && reg == REG::NUM) cpu_.push(val, CPU<>::MemoryStorage::RAM);
					else if (                          reg != REG::NUM) cpu_.push(reg, CPU<>::MemoryStorage::STACK);
					else		                                        cpu_.push(val, CPU<>::MemoryStorage::STACK);
				}
				else if (pOp->cmd == "pop")
				{
					if (pOp->args[0][0] == '[') cpu_.pop(CPU<>::MemoryStorage::RAM);
					else                        cpu_.pop(CPU<>::MemoryStorage::STACK);
				}

				else if (pOp->cmd == "add") cpu_.add();
				else if (pOp->cmd == "sub") cpu_.sub();
				else if (pOp->cmd == "mul") cpu_.mul();
				else if (pOp->cmd == "div") cpu_.div();
				else if (pOp->cmd == "dup") cpu_.dup();
				else if (pOp->cmd == "sin") cpu_.sin();
				else if (pOp->cmd == "cos") cpu_.cos();
				else if (pOp->cmd == "sqrt") cpu_.sqrt();

				else if (pOp->cmd == "dump") cpu_.dump();

				else if (pOp->cmd == "cmp")  cpu_.push(getValue(pOp->args[0]), CPU<>::MemoryStorage::STACK), 
					                         cpu_.push(getValue(pOp->args[1]), CPU<>::MemoryStorage::STACK);
				else if (pOp->cmd == "jump") Move2Label(file, pOp->args[0]);

				else if (pOp->cmd == "je")  { auto pair = cpu_.getPair(); if (pair.first == pair.second) Move2Label(file, pOp->args[0]); }
				else if (pOp->cmd == "jne") { auto pair = cpu_.getPair(); if (pair.first != pair.second) Move2Label(file, pOp->args[0]); }
				else if (pOp->cmd == "ja")  { auto pair = cpu_.getPair(); if (pair.first  > pair.second) Move2Label(file, pOp->args[0]); }
				else if (pOp->cmd == "jae") { auto pair = cpu_.getPair(); if (pair.first >= pair.second) Move2Label(file, pOp->args[0]); }
				else if (pOp->cmd == "jb")  { auto pair = cpu_.getPair(); if (pair.first  < pair.second) Move2Label(file, pOp->args[0]); }
				else if (pOp->cmd == "jbe") { auto pair = cpu_.getPair(); if (pair.first <= pair.second) Move2Label(file, pOp->args[0]); }

				else if (pOp->cmd == "move")
				{
					BOOL isArg0Reg = isReg(pOp->args[0]),
						 isArg1Reg = isReg(pOp->args[1]);

					if      ( isArg0Reg &&  isArg1Reg) cpu_.move(makeReg(pOp->args[0]), makeReg(pOp->args[1]));
					else if (!isArg0Reg &&  isArg1Reg) cpu_.move(getValue(pOp->args[0]), makeReg(pOp->args[1]));
				}

				else if (pOp->cmd == "end") { delete pOp; break; }

				else if (pOp->cmd[0] == ':' || !pOp->cmd.length()) {}

				else
				{
					NDebugger::Error("Unknown command: " + pOp->cmd);

					delete pOp;
					file.close();

					return FALSE;
				}
			}
			else break;

			delete pOp;
		}

		file.close();

		return TRUE;
	}

	template<typename T>
	BOOL Compiler<T>::fromComFile(CRSTRING filename)
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

			Operation *pOp = ParseCode(tmp);
			if (pOp)
			{
				switch (getValue(pOp->cmd))
				{
				case commands_::push:
				{
					auto val = getValue(pOp->args[0]);
					auto reg = makeReg(pOp->args[0]);

					if      (pOp->args[0][0] == '[' && reg != REG::NUM) cpu_.push(reg, CPU<>::MemoryStorage::RAM);
					else if (pOp->args[0][0] == '[' && reg == REG::NUM) cpu_.push(val, CPU<>::MemoryStorage::RAM);
					else if (                          reg != REG::NUM) cpu_.push(reg, CPU<>::MemoryStorage::STACK);
					else		                                        cpu_.push(val, CPU<>::MemoryStorage::STACK);

					break;
				}
				case commands_::pop:  
				{
					if (pOp->args[0][0] == '[') cpu_.pop(CPU<>::MemoryStorage::RAM);
					else                        cpu_.pop(CPU<>::MemoryStorage::STACK);

					break;
				}  

				case commands_::add:  cpu_.add();  break;
				case commands_::sub:  cpu_.sub();  break;
				case commands_::mul:  cpu_.mul();  break;
				case commands_::div:  cpu_.div();  break;
				case commands_::sqrt: cpu_.sqrt(); break;
				case commands_::dup:  cpu_.dup();  break;
				case commands_::sin:  cpu_.sin();  break;
				case commands_::cos:  cpu_.cos();  break;

				case commands_::dump: cpu_.dump(); break;

				case commands_::cmp: 
					cpu_.push(getValue(pOp->args[0]), CPU<>::MemoryStorage::STACK), 
					cpu_.push(getValue(pOp->args[1]), CPU<>::MemoryStorage::STACK); 
					break;
				case commands_::jump: Move2Label(file, pOp->args[0]);  break;

				case commands_::je:  { auto pair = cpu_.getPair(); if (pair.first == pair.second) Move2Label(file, pOp->args[0]); break; }
				case commands_::jne: { auto pair = cpu_.getPair(); if (pair.first != pair.second) Move2Label(file, pOp->args[0]); break; }
				case commands_::ja:  { auto pair = cpu_.getPair(); if (pair.first >  pair.second) Move2Label(file, pOp->args[0]); break; }
				case commands_::jae: { auto pair = cpu_.getPair(); if (pair.first >= pair.second) Move2Label(file, pOp->args[0]); break; }
				case commands_::jb:  { auto pair = cpu_.getPair(); if (pair.first <  pair.second) Move2Label(file, pOp->args[0]); break; }
				case commands_::jbe: { auto pair = cpu_.getPair(); if (pair.first <= pair.second) Move2Label(file, pOp->args[0]); break; }

				case commands_::move:
				{
					BOOL isArg0Reg = isReg(pOp->args[0]),
						 isArg1Reg = isReg(pOp->args[1]);

					if      ( isArg0Reg &&  isArg1Reg) cpu_.move(makeReg(pOp->args[0]), makeReg(pOp->args[1]));
					else if (!isArg0Reg &&  isArg1Reg) cpu_.move(getValue(pOp->args[0]), makeReg(pOp->args[1]));

					break;
				}

				case commands_::end: { delete pOp; file.close(); return TRUE; }

				case commands_::undefined: break;

				default:
				{
					NDebugger::Error("Unknown command: " + pOp->cmd);

					delete pOp;
					file.close();

					return FALSE;
				}
				}
			}
			else break;

			delete pOp;
		}

		file.close();

		return TRUE;
	}
	
	template<typename T>
	BOOL Compiler<T>::fromBinFile(CRSTRING filename)
	{
		std::ifstream file(filename + ".txt", std::ios::binary);
		if (!file.is_open())
		{
			NDebugger::Error("Cannot open file: " + filename);

			return FALSE;
		}

		while (!file.eof())
		{
			Wrap2BinaryIO<WORD> command;
			file >> command;

			switch (command)
				{
				case commands_::push:
				{
					Wrap2BinaryIO<std::string> arg;
					auto val = getValue(arg);
					auto reg = makeReg(arg);

					if (arg.operator const std::string &()[0] == '[')
					{
						NDebugger::Error("Not released yet");

						throw;
					}
					else if (reg != REG::NUM) cpu_.push(reg);
					else		              cpu_.push(val);

					break;
				}
				case commands_::pop:  cpu_.pop();  break;

				case commands_::add:  cpu_.add();  break;
				case commands_::sub:  cpu_.sub();  break;
				case commands_::mul:  cpu_.mul();  break;
				case commands_::div:  cpu_.div();  break;
				case commands_::sqrt: cpu_.sqrt(); break;
				case commands_::dup:  cpu_.dup();  break;
				case commands_::sin:  cpu_.sin();  break;
				case commands_::cos:  cpu_.cos();  break;

				case commands_::dump: cpu_.dump(); break;

				//case commands_::cmp: cpu_.push(getValue(pOp->args[0])), cpu_.push(getValue(pOp->args[1])); break;
				//case commands_::jump: Move2Label(file, pOp->args[0]);  break;

				//case commands_::je:  { auto pair = cpu_.getPair(); if (pair.first == pair.second) Move2Label(file, pOp->args[0]); break; }
				//case commands_::jne: { auto pair = cpu_.getPair(); if (pair.first != pair.second) Move2Label(file, pOp->args[0]); break; }
				//case commands_::ja:  { auto pair = cpu_.getPair(); if (pair.first > pair.second) Move2Label(file, pOp->args[0]); break; }
				//case commands_::jae: { auto pair = cpu_.getPair(); if (pair.first >= pair.second) Move2Label(file, pOp->args[0]); break; }
				//case commands_::jb:  { auto pair = cpu_.getPair(); if (pair.first < pair.second) Move2Label(file, pOp->args[0]); break; }
				//case commands_::jbe: { auto pair = cpu_.getPair(); if (pair.first <= pair.second) Move2Label(file, pOp->args[0]); break; }

				//case commands_::move:
				//{
				//	BOOL isArg0Reg = isReg(pOp->args[0]),
				//		 isArg1Reg = isReg(pOp->args[1]);

				//  if      ( isArg0Reg &&  isArg1Reg) cpu_.move(makeReg(pOp->args[0]), makeReg(pOp->args[1]));
	            //  else if (!isArg0Reg &&  isArg1Reg) cpu_.move(getValue(pOp->args[0]), makeReg(pOp->args[1]));
					
				//	break;
				//}

				case commands_::end: { file.close(); return TRUE; }

				case commands_::undefined: break;

				default:
				{
					NDebugger::Error("Unknown command: " + std::to_string(command));

					file.close();

					return FALSE;
				}
				}
		}

		file.close();

		return TRUE;
	}
	 
#pragma endregion
} // NCompiler

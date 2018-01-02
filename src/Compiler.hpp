#pragma once

#include <fstream>
#include <sstream>

#include "Parser.hpp"
#include "CPU.hpp"

namespace NCompiler
{
#pragma region Class Wrap4BinaryIO
	
	template<typename T>
	class Wrap4BinaryIO final
	{
	public:
		Wrap4BinaryIO();
		Wrap4BinaryIO(CONST T&);
		~Wrap4BinaryIO();

		operator T&();
		operator CONST T&() const;

	private:
		T val_;
	};

	//====================================================================================================================================

	template<typename T>
	std::istream &operator>>(std::istream&, Wrap4BinaryIO<T>&);

	template<typename T>
	std::ostream &operator<<(std::ostream&, Wrap4BinaryIO<T>&);

	//====================================================================================================================================

	template<typename T>
	inline Wrap4BinaryIO<T>::Wrap4BinaryIO() :
		val_()
	{ }

	template<typename T>
	inline Wrap4BinaryIO<T>::Wrap4BinaryIO(CONST T &crVal) :
		val_(crVal)
	{ }

	template<typename T>
	inline Wrap4BinaryIO<T>::~Wrap4BinaryIO()
	{ }

	template<typename T>
	inline Wrap4BinaryIO<T>::operator T&()
	{
		return val_;
	}

	template<typename T>
	inline Wrap4BinaryIO<T>::operator CONST T&() const
	{
		return val_;
	}

	//====================================================================================================================================

	template<typename T>
	inline std::istream &operator>>(std::istream &rIstr, Wrap4BinaryIO<T> &rVal)
	{
		rIstr.read(reinterpret_cast<CHAR*>(&static_cast<T&>(rVal)), sizeof(T));

		return rIstr;
	}

	template<> 
	std::istream &operator>><std::string>(std::istream &rIstr, Wrap4BinaryIO<std::string> &rVal)
	{
		SIZE_T size = 0;
		rIstr.read(reinterpret_cast<CHAR*>(&size), sizeof(size));
		
		CHAR *pBuf = new CHAR[size + 1]();
		rIstr.read(pBuf, size);
		pBuf[size] = '\0';

		rVal = std::string(pBuf);
		
		delete[] pBuf;

		return rIstr;
	}

	template<typename T>
	std::ostream &operator<<(std::ostream &rOstr, Wrap4BinaryIO<T> &rVal)
	{
		rOstr.write(reinterpret_cast<CONST CHAR*>(&static_cast<T&>(rVal)), sizeof(T));

		return rOstr;
	}

	template<>
	std::ostream &operator<<<std::string>(std::ostream &rOstr, Wrap4BinaryIO<std::string> &rVal)
	{	
		SIZE_T size = rVal.operator CRSTRING().length();
		rOstr.write(reinterpret_cast<CONST CHAR*>(&size), sizeof(size));
		rOstr.write(rVal.operator CRSTRING().c_str(), size);

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

#pragma warning(push)
#pragma warning(disable:4238) // Standard extension: use of the right-hand class value as a left-handed value in defines

#define WRITE_STRING(str) Wrap4BinaryIO<std::string>(str)
#define ARG(index)        WRITE_STRING(pOp->args[index])

		while (!input.eof())
		{
			CHAR tmp[MAX_LINE_LENGTH] = {};
			input.getline(tmp, MAX_LINE_LENGTH, '\n');

			Operation *pOp = ParseCode(tmp);
			if (pOp)
			{
				if(!pOp->cmd.length()) continue;

				else if (pOp->cmd == "push") output << WRITE_STRING(pOp->cmd) << ARG(0);
				else if (pOp->cmd == "pop")  output << WRITE_STRING(pOp->cmd) << ARG(0);

				else if (pOp->cmd == "add")	 output << WRITE_STRING(pOp->cmd);
				else if (pOp->cmd == "sub")  output << WRITE_STRING(pOp->cmd);
				else if (pOp->cmd == "mul")	 output << WRITE_STRING(pOp->cmd);
				else if (pOp->cmd == "div")	 output << WRITE_STRING(pOp->cmd);
				else if (pOp->cmd == "dup")	 output << WRITE_STRING(pOp->cmd);
				else if (pOp->cmd == "sin")  output << WRITE_STRING(pOp->cmd);
				else if (pOp->cmd == "cos")  output << WRITE_STRING(pOp->cmd);
				else if (pOp->cmd == "sqrt") output << WRITE_STRING(pOp->cmd);

				else if (pOp->cmd == "dump") output << WRITE_STRING(pOp->cmd);

				else if (pOp->cmd == "cmp")  output << WRITE_STRING(pOp->cmd) << ARG(0) << ARG(1);
				else if (pOp->cmd == "jump") output << WRITE_STRING(pOp->cmd) << ARG(0);

				else if (pOp->cmd == "je")	 output << WRITE_STRING(pOp->cmd) << ARG(0);
				else if (pOp->cmd == "jne")  output << WRITE_STRING(pOp->cmd) << ARG(0);
				else if (pOp->cmd == "ja")	 output << WRITE_STRING(pOp->cmd) << ARG(0);
				else if (pOp->cmd == "jae")	 output << WRITE_STRING(pOp->cmd) << ARG(0);
				else if (pOp->cmd == "jb")	 output << WRITE_STRING(pOp->cmd) << ARG(0);
				else if (pOp->cmd == "jbe")  output << WRITE_STRING(pOp->cmd) << ARG(0);

				else if (pOp->cmd == "move") output << WRITE_STRING(pOp->cmd) << ARG(0) << ARG(1);

				else if (pOp->cmd == "end")  output << WRITE_STRING(pOp->cmd);

				else if (pOp->cmd[0] == ':' || !pOp->cmd.length()) output << WRITE_STRING(pOp->cmd);

				else
				{
					NDebugger::Error("Unknown command: " + std::string(tmp));

					delete pOp;

					output.close();
					input.close();

					return FALSE;
				}


			}
			else break;
			
			delete pOp;
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

				else if (pOp->cmd[0] == ':') {} // Push to func stack

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

#define COMMAND command.operator CRSTRING()

		while (!file.eof())
		{
			Wrap4BinaryIO<std::string> command;
			file >> command;

			if (COMMAND == "push")
			{
				file >> command;
				auto val = getValue(command);
				auto reg = makeReg(command);

				if      (COMMAND[0] == '[' && reg != REG::NUM) cpu_.push(reg, CPU<>::MemoryStorage::RAM);
				else if (COMMAND[0] == '[' && reg == REG::NUM) cpu_.push(val, CPU<>::MemoryStorage::RAM);
				else if (                     reg != REG::NUM) cpu_.push(reg, CPU<>::MemoryStorage::STACK);
				else		                                   cpu_.push(val, CPU<>::MemoryStorage::STACK);
			}
			else if (COMMAND == "pop")
			{
				file >> command;
				if (COMMAND[0] == '[') cpu_.pop(CPU<>::MemoryStorage::RAM);
				else                   cpu_.pop(CPU<>::MemoryStorage::STACK);
			}

			else if (COMMAND == "add") cpu_.add();
			else if (COMMAND == "sub") cpu_.sub();
			else if (COMMAND == "mul") cpu_.mul();
			else if (COMMAND == "div") cpu_.div();
			else if (COMMAND == "dup") cpu_.dup();
			else if (COMMAND == "sin") cpu_.sin();
			else if (COMMAND == "cos") cpu_.cos();
			else if (COMMAND == "sqrt") cpu_.sqrt();

			else if (COMMAND == "dump") cpu_.dump();

			else if (COMMAND == "cmp")
			{
				file >> command; //-V760
				cpu_.push(getValue(command), CPU<>::MemoryStorage::STACK);
				
				file >> command;
				cpu_.push(getValue(command), CPU<>::MemoryStorage::STACK);
			}
			//else if (command == "jump") Move2Label(file, pOp->args[0]);

			//else if (command == "je") { auto pair = cpu_.getPair(); if (pair.first == pair.second) Move2Label(file, pOp->args[0]); }
			//else if (command == "jne") { auto pair = cpu_.getPair(); if (pair.first != pair.second) Move2Label(file, pOp->args[0]); }
			//else if (command == "ja") { auto pair = cpu_.getPair(); if (pair.first  > pair.second) Move2Label(file, pOp->args[0]); }
			//else if (command == "jae") { auto pair = cpu_.getPair(); if (pair.first >= pair.second) Move2Label(file, pOp->args[0]); }
			//else if (pOp->cmd == "jb") { auto pair = cpu_.getPair(); if (pair.first  < pair.second) Move2Label(file, pOp->args[0]); }
			//else if (pOp->cmd == "jbe") { auto pair = cpu_.getPair(); if (pair.first <= pair.second) Move2Label(file, pOp->args[0]); }

			else if (COMMAND == "move")
			{
				std::string arg0, 
					        arg1;
				file >> arg0 >> arg1;

				BOOL isArg0Reg = isReg(arg0),
					 isArg1Reg = isReg(arg1);

				if      ( isArg0Reg &&  isArg1Reg) cpu_.move( makeReg(arg0), makeReg(arg1));
				else if (!isArg0Reg &&  isArg1Reg) cpu_.move(getValue(arg0), makeReg(arg1));
			}

			else if (COMMAND == "end") break;

			else if (COMMAND[0] == ':') {} // Push to func stack

			else
			{
				NDebugger::Error("Unknown command: " + COMMAND);

				file.close();

				return FALSE;
			}
		}

		file.close();

		return TRUE;
	}
	 
#pragma endregion
} // NCompiler

#pragma once

//#include <boost\serialization\serialization.hpp>
// TODO: Make binary input/output with boost

#include <fstream>
#include <sstream>

#include "Parser.hpp"
#include "CPU.hpp"

namespace NCompiler
{
	using namespace NParser;

	using NCpu::REG;
	using NCpu::CPU;

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
		//BOOL toBinFile(CRSTRING) const;

		BOOL fromTextFile(CRSTRING);
		BOOL fromComFile(CRSTRING);
		//BOOL fromBinFile(CRSTRING);
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

	/*template<typename T>
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

#define WRITE(what) output.write(reinterpret_cast<CHAR*>(what), sizeof(what))
#define WRITES(what) output.write(what.c_str(), what.length())
#define SPACE WRITE(' ')
#define ENDL WRITE('\n')

				if      (pOp->cmd == "push") WRITE(commands_::push), SPACE, WRITES(pOp->args[0]); 
				else if (pOp->cmd == "pop")  WRITE(commands_::pop),  SPACE, WRITES(pOp->args[0]);

				else if (pOp->cmd == "add")	 WRITE(commands_::add);
				else if (pOp->cmd == "sub")  WRITE(commands_::sub);
				else if (pOp->cmd == "mul")	 WRITE(commands_::mul);
				else if (pOp->cmd == "div")	 WRITE(commands_::div);
				else if (pOp->cmd == "dup")	 WRITE(commands_::dup);
				else if (pOp->cmd == "sin")  WRITE(commands_::sin);
				else if (pOp->cmd == "cos")  WRITE(commands_::cos);
				else if (pOp->cmd == "sqrt") WRITE(commands_::sqrt);

				else if (pOp->cmd == "dump") WRITE(commands_::dump);

				else if (pOp->cmd == "cmp")  WRITE(commands_::cmp),  SPACE, WRITES(pOp->args[0]), SPACE, WRITES(pOp->args[1]);
				else if (pOp->cmd == "jump") WRITE(commands_::jump), SPACE, WRITES(pOp->args[0]);

				else if (pOp->cmd == "je")	 WRITE(commands_::je),   SPACE, WRITES(pOp->args[0]);
				else if (pOp->cmd == "jne")  WRITE(commands_::jne),  SPACE, WRITES(pOp->args[0]);
				else if (pOp->cmd == "ja")	 WRITE(commands_::ja),   SPACE, WRITES(pOp->args[0]);
				else if (pOp->cmd == "jae")	 WRITE(commands_::jae),  SPACE, WRITES(pOp->args[0]);
				else if (pOp->cmd == "jb")	 WRITE(commands_::jb),   SPACE, WRITES(pOp->args[0]);
				else if (pOp->cmd == "jbe")  WRITE(commands_::jbe),  SPACE, WRITES(pOp->args[0]);

				else if (pOp->cmd == "move") WRITE(commands_::move), SPACE, WRITES(pOp->args[0]), SPACE, WRITES(pOp->args[1]);

				else if (pOp->cmd == "end")  WRITE(commands_::end);

				else if (pOp->cmd[0] == ':' || !pOp->cmd.length()) WRITES(pOp->cmd);

				else
				{
					NDebugger::Error("Unknown command: " + std::string(tmp));

					delete pOp;

					output.close();
					input.close();

					return FALSE;
				}

				ENDL;
#undef ENDL
#undef SPACE
#undef WRITES
#undef WRITE
			}
			else break;
			
			delete pOp;
		}

		output.close();
		input.close();

		return TRUE;
	}*/

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

					if      (pOp->args[0][0] == '[' && reg != REG::NUM) cpu_.put(reg);
					else if (pOp->args[0][0] == '[' && reg == REG::NUM) cpu_.put(val);
					else if (                          reg != REG::NUM) cpu_.push(reg);
					else		                                        cpu_.push(val);
				}
				else if (pOp->cmd == "pop")
				{
					if (pOp->args[0][0] == '[') cpu_.popm();
					else                        cpu_.pop();
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

				else if (pOp->cmd == "cmp")  cpu_.push(getValue(pOp->args[0])), cpu_.push(getValue(pOp->args[1]));
				else if (pOp->cmd == "jump") { file = FindLabel(file, pOp->args[0]); }

				else if (pOp->cmd == "je")  { auto pair = cpu_.getPair(); if (pair.first == pair.second) file = FindLabel(file, pOp->args[0]); }
				else if (pOp->cmd == "jne") { auto pair = cpu_.getPair(); if (pair.first != pair.second) file = FindLabel(file, pOp->args[0]); }
				else if (pOp->cmd == "ja")  { auto pair = cpu_.getPair(); if (pair.first  > pair.second) file = FindLabel(file, pOp->args[0]); }
				else if (pOp->cmd == "jae") { auto pair = cpu_.getPair(); if (pair.first >= pair.second) file = FindLabel(file, pOp->args[0]); }
				else if (pOp->cmd == "jb")  { auto pair = cpu_.getPair(); if (pair.first  < pair.second) file = FindLabel(file, pOp->args[0]); }
				else if (pOp->cmd == "jbe") { auto pair = cpu_.getPair(); if (pair.first <= pair.second) file = FindLabel(file, pOp->args[0]); }

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

					if (pOp->args[0][0] == '[')
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

				case commands_::cmp: cpu_.push(getValue(pOp->args[0])), cpu_.push(getValue(pOp->args[1])); break;
				case commands_::jump: file = FindLabel(file, pOp->args[0]);  break;

				case commands_::je:  { auto pair = cpu_.getPair(); if (pair.first == pair.second) file = FindLabel(file, pOp->args[0]); break; }
				case commands_::jne: { auto pair = cpu_.getPair(); if (pair.first != pair.second) file = FindLabel(file, pOp->args[0]); break; }
				case commands_::ja:  { auto pair = cpu_.getPair(); if (pair.first > pair.second) file = FindLabel(file, pOp->args[0]); break; }
				case commands_::jae: { auto pair = cpu_.getPair(); if (pair.first >= pair.second) file = FindLabel(file, pOp->args[0]); break; }
				case commands_::jb:  { auto pair = cpu_.getPair(); if (pair.first < pair.second) file = FindLabel(file, pOp->args[0]); break; }
				case commands_::jbe: { auto pair = cpu_.getPair(); if (pair.first <= pair.second) file = FindLabel(file, pOp->args[0]); break; }

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
} // NCompiler

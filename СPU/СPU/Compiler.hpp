#pragma once

#include "Parser.hpp"
#include "CPU.hpp"

template<typename T = INT>
class Compiler final
{
	CPU<T> cpu_;
	
	T getValue(CONST std::string &crVal) const // TODO: change return value type
	{ 
		std::stringstream sstr(crVal);
		
		T val = NULL; 
		sstr >> val; 
		
		return val; 
	}

	REG makeReg(CONST std::string &crStrReg) const
	{
		if      (crStrReg == "ax") return REG::AX;
		else if (crStrReg == "bx") return REG::BX;
		else if (crStrReg == "cx") return REG::CX;
		else if (crStrReg == "dx") return REG::DX;
		else if (crStrReg == "ex") return REG::EX;
		else if (crStrReg == "sp") return REG::SP;

		else                       return REG::NUM;
	}

	BOOL isReg(CONST std::string &val) const { return makeReg(val) != REG::NUM; }

public:
	enum commands_ : UNSIGNED
	{
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

		in, // What is this???
		out, // What is this???
		swi,

		end
	};

	explicit Compiler() : cpu_() { }
	Compiler(CONST Compiler<T> &crComp) : cpu_(crComp.cpu_) { }
	Compiler(Compiler<T> &&rrComp) : cpu_(std:move(rrComp.cpu_)) { }
	~Compiler() { }

	Compiler<T> &operator=(CONST Compiler<T>&);
	Compiler<T> &operator=(Compiler<T>&&);
	
	BOOL toComTextFile(CRSTRING) const;
	//BOOL toBinFile(CRSTRING) const;

	BOOL FromTextFile(CRSTRING);
	BOOL FromComTextFile(CRSTRING);
	//BOOL FromBinFile(CRSTRING);
};

template<typename T>
Compiler<T> &Compiler<T>::operator=(CONST Compiler<T> &crComp)
{
    if (this != &crComp) cpu_ = crComp.cpu_;

    return *this;
}

template<typename T>
Compiler<T> &Compiler<T>::operator=(Compiler<T> &&rrComp)
{
    assert(this != &rrComp);

    cpu_ = std::move(rrComp.cpu_);

    return *this;
}

template<typename T>
BOOL Compiler<T>::toComTextFile(CRSTRING filename) const 
{
	std::ifstream input(filename + ".txt");
	if (!input.is_open())
	{
		Debugger::Error("Cannot open file: " + filename);

		return FALSE;
	}

	std::string outputFile = filename + "ComText" + ".txt";
	std::ofstream output(outputFile);
	if (!output.is_open())
	{
		Debugger::Error("Cannot open file: " + outputFile);

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
			else if (pOp->cmd == "pop")  output << commands_::pop;

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

			else if (pOp->cmd == "je")	 output << commands_::je   << " " << pOp->args[0];
			else if (pOp->cmd == "jne")  output << commands_::jne  << " " << pOp->args[0];
			else if (pOp->cmd == "ja")	 output << commands_::ja   << " " << pOp->args[0];
			else if (pOp->cmd == "jae")	 output << commands_::jae  << " " << pOp->args[0];
			else if (pOp->cmd == "jb")	 output << commands_::jb   << " " << pOp->args[0];
			else if (pOp->cmd == "jbe")  output << commands_::jbe  << " " << pOp->args[0];

			else if (pOp->cmd == "move") output << commands_::move << " " << pOp->args[0] << " " << pOp->args[1];

			else if (pOp->cmd == "in")	 output << commands_::in;
			else if (pOp->cmd == "out")  output << commands_::out;
			else if (pOp->cmd == "swi")  output << commands_::swi;

			else if (pOp->cmd == "end")  output << commands_::end;

			else if (pOp->cmd[0] == ':' || !pOp->cmd.length()) output << pOp->cmd;

			else
			{
				Debugger::Error("Unknown command: " + std::string(tmp));

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

//===================================================================================================================

template<typename T>
BOOL Compiler<T>::FromTextFile(CRSTRING filename)
{
	std::ifstream file(filename + ".txt");
	if (!file.is_open())
	{
		Debugger::Error("Cannot open file: " + filename);

		return FALSE;
	}

	while (!file.eof())
	{
		CHAR tmp[MAX_LINE_LENGTH] = { };
		file.getline(tmp, MAX_LINE_LENGTH, '\n');

		Operation *pOp = ParseCode(tmp);
		if (pOp)
		{
			if      (pOp->cmd == "push") cpu_.push(getValue(pOp->args[0]));
			else if (pOp->cmd == "pop")  cpu_.pop();

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
			else if (pOp->cmd == "ja")  { auto pair = cpu_.getPair(); if (pair.first >  pair.second) file = FindLabel(file, pOp->args[0]); }
			else if (pOp->cmd == "jae") { auto pair = cpu_.getPair(); if (pair.first >= pair.second) file = FindLabel(file, pOp->args[0]); }
			else if (pOp->cmd == "jb")  { auto pair = cpu_.getPair(); if (pair.first <  pair.second) file = FindLabel(file, pOp->args[0]); }
			else if (pOp->cmd == "jbe") { auto pair = cpu_.getPair(); if (pair.first <= pair.second) file = FindLabel(file, pOp->args[0]); }

			else if(pOp->cmd == "move")
			{
				BOOL isArg0Reg = isReg(pOp->args[0]),
					 isArg1Reg = isReg(pOp->args[1]);
				
				if      ( isArg0Reg &&  isArg1Reg) cpu_.move( makeReg(pOp->args[0]),  makeReg(pOp->args[1]));
				else if (!isArg0Reg &&  isArg1Reg) cpu_.move(getValue(pOp->args[0]),  makeReg(pOp->args[1]));
				//else if ( isArg0Reg && !isArg1Reg) cpu_.move( makeReg(pOp->args[0]), getValue(pOp->args[1]));
				//else                               cpu_.move(getValue(pOp->args[0]), getValue(pOp->args[1]));
			}

			//else if (pOp->cmd == "in");
			//else if (pOp->cmd == "out");
			//else if (pOp->cmd == "swi");

			else if(pOp->cmd == "end") { delete pOp; break; }

			else if (pOp->cmd[0] == ':' || !pOp->cmd.length()) { }

			else
			{
				Debugger::Error("Unknown command: " + pOp->cmd);

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
BOOL Compiler<T>::FromComTextFile(CRSTRING filename)
{
	std::ifstream file(filename + ".txt");
	if (!file.is_open())
	{
		Debugger::Error("Cannot open file: " + filename);

		return FALSE;
	}

	while (!file.eof())
	{
		CHAR tmp[MAX_LINE_LENGTH] = { };
		file.getline(tmp, MAX_LINE_LENGTH, '\n');

		Operation *pOp = ParseCode(tmp);
		if (pOp)
		{
			switch (atoi(pOp->cmd.c_str()))
			{
			case commands_::push:
			{
				//CONST auto &val_str = pOp->args[0];
				//std::stringstream sstr(val_str);
				//T val = (std::is_pointer<T>::value ? nullptr : NULL);

				//sstr >> val;

				//cpu_.push(val); 

				break; // TODO: delete dat bad code
			}

			case commands_::pop:
				cpu_.pop(); 
				break;

			case commands_::add: 								
				cpu_.add();
				break;

			// TODO: Other functions

			case commands_::dump:
				cpu_.dump(); // QUEST: Why only dump works?
				break;

			default: 
			{
				Debugger::Error("Unknown command: " + pOp->cmd);

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

#ifdef DEBUG
	cpu_.dump();
#endif // DEBUG

	return TRUE;

}

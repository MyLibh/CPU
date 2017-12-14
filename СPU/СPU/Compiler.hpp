#pragma once

#include "Parser.hpp"
#include "CPU.hpp"

template<typename T = INT>
class Compiler final
{
	CPU<T> cpu_;

public:
	static CONST SIZE_T MAX_LINE_LENGTH = 1 << 8;

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

		jump,
		je,
		jne,
		ja,
		jae,
		jb,
		jbe,

		mov,

		in, // What is this???
		out, // What is this???
		swi,

		end
	};

	explicit Compiler() : cpu_() { }
	Compiler(CONST Compiler<T> &crComp) : cpu_(crComp.cpu_j { }
	Compiler(Compiler<T> &&rrComp) : cpu_(std:move(rrComp.cpu_)) { }
	~Compiler() { }

	Compiler<T> &operator=(CONST Compiler<T>&);
	Compiler<T> &operator=(Compiler<T>&&);
	
	BOOL toComTextFile(CONST std::string&) const;
	//BOOL toBinFile(CONST std::string&) const;

	BOOL FromTextFile(CONST std::string&);
	BOOL FromComTextFile(CONST std::string&);
	//BOOL FromBinFile(CONST std::string&);
};

template<typename T>
Compiler &Compiler<T>::operator=(CONST Compiler<T> &crComp)
{
    if (this != &crComp) cpu_ = crComp.cpu_;

    return *this;
}

template<typename T>
Compiler &Compiler<T>::operator=(Compiler<T> &&rrComp)
{
    assert(this != &rrComp);

    cpu_ = std::move(rrComp.cpu_);

    return *this;
}

template<typename T>
BOOL Compiler<T>::toComTextFile(CONST std::string &crFilename) const
{
	std::ifstream input(crFilename + ".txt");
	if (!input.is_open())
	{
		std::cerr << "Cannot open file: " << crFilename << std::endl;

		input.close();

		return FALSE;
	}

	std::string outputFile = crFilename + "ComText" + ".txt";
	std::ofstream output(outputFile);
	if (!output.is_open())
	{
		std::cerr << "Cannot open file: " << outputFile << std::endl;

		input.close();
		output.close();

		return FALSE;
	}

	while (!input.eof())
	{
		std::string tmp;
		input >> tmp;

		if (tmp == "push")
		{
			T val = NULL;
			input >> val;

			output << commands_::pop << " " << val;
		}
		else if (tmp == "pop")   output << commands_::pop;

		else if (tmp == "add")	 output << commands_::add;
		else if (tmp == "sub")   output << commands_::sub;
		else if (tmp == "mul")	 output << commands_::mul;
		else if (tmp == "div")	 output << commands_::div;
		else if (tmp == "dup")	 output << commands_::dup;
		else if (tmp == "sin")   output << commands_::sin;
		else if (tmp == "cos")   output << commands_::cos;
		else if (tmp == "sqrt")  output << commands_::sqrt;

		else if (tmp == "dump")  output << commands_::dump;

		//else if (tmp == "jump")  output << commands_::jump;
		//else if (tmp == "je")	 output << commands_::je;
		//else if (tmp == "jne")   output << commands_::jne;
		//else if (tmp == "ja")	 output << commands_::ja;
		//else if (tmp == "jae")	 output << commands_::jae;
		//else if (tmp == "jb")	 output << commands_::jb;
		//else if (tmp == "jbe")   output << commands_::jbe;

		//else if (tmp == "move")	 output << commands_::move;

		//else if (tmp == "in")	 output << commands_::in;
		//else if (tmp == "out")   output << commands_::out;
		//else if (tmp == "swi")   output << commands_::swi;

		else if (tmp == "end")   output << commands_::end;

		else 
		{ 
			std::cerr << "Unknown command: " << tmp << std::endl; 
			
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

/*
template<typename T>
BOOL Compiler<T>::toBinFile(CONST std::string &crFilename) const
{
	std::ifstream input(crFilename + ".txt");
	if (!input.is_open())
	{
		std::cerr << "Cannot open file: " << crFilename << std::endl;

		return FALSE;
	}

	std::string outputFile = crFilename + "Bin" + ".txt";
	std::ofstream output(outputFile, std::ios::binary);
	if (!output.is_open())
	{
		std::cerr << "Cannot open file: " << outputFile << std::endl;

		return FALSE;
	}

	while (!input.eof())
	{
		std::string tmp;
		input >> tmp;
		

		else
		{
			std::cerr << "Unknown command: " << tmp << std::endl;

			output.close();
			input.close();

			return FALSE;
		}

		output << std::endl;
	}
	output.close();
	input.close();

	return TRUE;
} */

template<typename T>
BOOL Compiler<T>::FromTextFile(CONST std::string&) const
{
	std::ifstream file(crFilename + ".txt");
	if (!file.is_open())
	{
		std::cerr << "Cannot open file: " << crFilename << std::endl;

		file.close();

		return FALSE;
	}

	while (!file.eof())
	{
		CHAR tmp[MAX_LINE_LENGTH] = { };
		file.getline(tmp, MAX_LINE_LENGTH, '\n');

		Operation *pOp = ParseCode(tmp);
		if (pOp)
		{
			if      (pOp->cmd == "push") cpu_.push(atoi(pOp->args[0].c_str()));
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

			// TODO: Other commands

			else
			{
				std::cerr << "Unknown command: " << pOp->cmd << std::endl;

				delete pOp;
				file.close();

				return FALSE;
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

template<typename T>
BOOL Compiler<T>::FromComTextFile(CONST std::string &crFilename) const
{
	std::ifstream file(crFilename + ".txt");
	if (!file.is_open())
	{
		std::cerr << "Cannot open file: " << crFilename << std::endl;

		file.close();

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
				//cpu_.pop(); 
				break;

			case commands_::add: 								
				//cpu_.add();
				break;

			// TODO: Other functions

			case commands_::dump:
				cpu_.dump(); // QUEST: Why only dump works?
				break;

			default: 
			{
				std::cerr << "Unknown command: " << pOp->cmd << std::endl;

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

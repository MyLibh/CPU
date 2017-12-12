#pragma once

#include <string>
#include <fstream>
#include <iostream>

#include "MyTypedefs.hpp"

template<typename T = INT>
struct Compiler final
{
	enum commands_ : INT
	{
		// Stack commands
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

		// CPU commands
		jump,
		je,
		jne,
		ja,
		jae,
		jb,
		jbe,

		end
	};

	explicit Compiler() = default;
	Compiler(CONST Compiler<T>&) = default;
	Compiler(Compiler<T>&&) = default;
	~Compiler() = default;

	Compiler<T> &operator=(CONST Compiler<T>&) = default;
	Compiler<T> &operator=(Compiler<T>&&) = default;
	
	BOOL toTextFile(CONST std::string&) const;
	//BOOL toBinFile(CONST std::string&) const;
};

template<typename T>
BOOL Compiler<T>::toTextFile(CONST std::string &crFilename) const
{
	std::ifstream input(crFilename + ".txt");
	if (!input.is_open())
	{
		std::cerr << "Cannot open file: " << crFilename << std::endl;

		return FALSE;
	}

	std::string outputFile = crFilename + "Text" + ".txt";
	std::ofstream output(outputFile);
	if (!output.is_open())
	{
		std::cerr << "Cannot open file: " << outputFile << std::endl;

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

		else if (tmp == "jump")  output << commands_::jump;
		else if (tmp == "je")	 output << commands_::je;
		else if (tmp == "jne")   output << commands_::jne;
		else if (tmp == "ja")	 output << commands_::ja;
		else if (tmp == "jae")	 output << commands_::jae;
		else if (tmp == "jb")	 output << commands_::jb;
		else if (tmp == "jbe")   output << commands_::jbe;

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

#pragma once

//====================================================================================================================================
//!
//!	\file   Wrap4BinaryIO.hpp
//!
//! \brief	Header file with a wrapper for binary input/output
//!
//====================================================================================================================================

#ifndef __cplusplus
#error
#error  Must use C++ to compile.
#error
#endif /* __cplusplus */

#include <memory>  // std::unique_ptr
#include <iosfwd>  // std::ifstream, std::ofstream
#include <cassert> // assert

#pragma region CLASSES

template<typename T> 
class Wrap4BinaryIO final
{
public:
	Wrap4BinaryIO()  = default;
	Wrap4BinaryIO(const T&) noexcept;
	Wrap4BinaryIO(T&&)      noexcept;
	~Wrap4BinaryIO() = default;

	Wrap4BinaryIO &operator=(const Wrap4BinaryIO&) noexcept;
	Wrap4BinaryIO &operator=(Wrap4BinaryIO&&)      noexcept;

	operator       T&()       noexcept;
	operator const T&() const noexcept;

private:
	T val_;
};

#pragma endregion

#pragma region FUNCTION_DECLARATION

template<typename T>
std::istream &operator>>(std::istream&, Wrap4BinaryIO<T>&);

template<typename T>
std::ostream &operator<<(std::ostream&, Wrap4BinaryIO<T>&);

#pragma endregion

#pragma region METHOD_DEFINITION

template<typename T>
inline Wrap4BinaryIO<T>::Wrap4BinaryIO(const T &crVal) noexcept :
	val_(crVal)
{ }

template<typename T>
inline Wrap4BinaryIO<T>::Wrap4BinaryIO(T &&rrVal) noexcept :
	val_(std::move(rrVal))
{ }

template<typename T>
inline Wrap4BinaryIO<T> &Wrap4BinaryIO<T>::operator=(const Wrap4BinaryIO<T> &crW4BIO) noexcept
{
	if (this != &crW4BIO)
	{
		val_ = crW4BIO.val_;
	}

	return (*this);
}

template<typename T>
inline Wrap4BinaryIO<T> &Wrap4BinaryIO<T>::operator=(Wrap4BinaryIO<T> &&rrW4BIO) noexcept
{
	assert(this != &rrW4BIO);

	val_ = std::move(rrW4BIO.val_);

	return (*this);
}

template<typename T>
inline Wrap4BinaryIO<T>::operator T&() noexcept
{
	return val_;
}

template<typename T>
inline Wrap4BinaryIO<T>::operator const T&() const noexcept
{
	return val_;
}

#pragma endregion

#pragma region TEMPLATE_SPECIALIZATION

template<>
std::istream &operator>><std::string>(std::istream &rIstr, Wrap4BinaryIO<std::string> &rVal)
{
	size_t size = 0;
	rIstr.read(reinterpret_cast<char*>(&size), sizeof(size));

	std::unique_ptr<char[]> buf(new char[size + 1]);
	rIstr.read(buf.get(), size);
	buf[size] = '\0';

	rVal = std::move(std::string(buf.get()));

	return rIstr;
}

template<>
std::ostream &operator<<<std::string>(std::ostream &rOstr, Wrap4BinaryIO<std::string> &rVal)
{
	size_t size = rVal.operator const std::string&().length();
	rOstr.write(reinterpret_cast<const char*>(&size), sizeof(size));
	rOstr.write(rVal.operator const std::string&().c_str(), size);

	return rOstr;
}

#pragma endregion

#pragma region FUNCTION_DEFINITION

template<typename T>
std::istream &operator>>(std::istream &rIstr, Wrap4BinaryIO<T> &rVal)
{
	rIstr.read(reinterpret_cast<char*>(&static_cast<T&>(rVal)), sizeof(T));

	return rIstr;
}

template<typename T>
std::ostream &operator<<(std::ostream &rOstr, Wrap4BinaryIO<T> &rVal)
{
	rOstr.write(reinterpret_cast<const char*>(&static_cast<T&>(rVal)), sizeof(T));

	return rOstr;
}

#pragma endregion



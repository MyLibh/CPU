#pragma once

#include <iosfwd>

#include "MyTypedefs.hpp"

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
std::istream &operator>>(std::istream &rIstr, Wrap4BinaryIO<T> &rVal)
{
	rIstr.read(reinterpret_cast<CHAR*>(&static_cast<T&>(rVal)), sizeof(T));

	return rIstr;
}

template<typename T>
std::ostream &operator<<(std::ostream &rOstr, Wrap4BinaryIO<T> &rVal)
{
	rOstr.write(reinterpret_cast<CONST CHAR*>(&static_cast<T&>(rVal)), sizeof(T));

	return rOstr;
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

template<>
std::ostream &operator<<<std::string>(std::ostream &rOstr, Wrap4BinaryIO<std::string> &rVal)
{
	SIZE_T size = rVal.operator CRSTRING().length();
	rOstr.write(reinterpret_cast<CONST CHAR*>(&size), sizeof(size));
	rOstr.write(rVal.operator CRSTRING().c_str(), size);

	return rOstr;
}



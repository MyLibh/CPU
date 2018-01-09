#pragma once

#include <iosfwd>

template<typename T>
class Wrap4BinaryIO final
{
public:
	Wrap4BinaryIO();
	Wrap4BinaryIO(const T&);
	~Wrap4BinaryIO();

	operator T&();
	operator const T&() const;

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
inline Wrap4BinaryIO<T>::Wrap4BinaryIO(const T &crVal) :
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
inline Wrap4BinaryIO<T>::operator const T&() const
{
	return val_;
}

//====================================================================================================================================

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



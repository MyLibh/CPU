#pragma once

//====================================================================================================================================
//!
//!	\file   Storage.hpp
//!
//! \brief	Implementation of a simple static storage
//!
//====================================================================================================================================

#ifndef __cplusplus
#error
#error  Must use C++ to compile.
#error
#endif /* __cplusplus */

#include <cassert> // assert
#include <iomanip> // std::setw
#include <array>   // std::array

#include "Guard.hpp"

#pragma region CLASSES

template<typename T, size_t SIZE>
class Storage 
{
public:
	typedef       T  &rVal_;
	typedef       T &&rrVal_;
	typedef const T  &crVal_;

	explicit Storage()      noexcept;
	Storage(const Storage&) noexcept;
	Storage(Storage&&)      noexcept;
	~Storage();

	virtual Storage &operator=(const Storage&) noexcept;
	virtual Storage &operator=(Storage&&)      noexcept;

//====================================================================================================================================
//!
//! \brief   Returnes element with index index(with size checking)
//! 
//! \param   index  Position of the element
//!
//! \return  Element at index 
//!
//! \throw   std::out_of_range
//!
//====================================================================================================================================

	virtual rVal_  operator[](size_t index);

//====================================================================================================================================
//!
//! \brief   Returnes element with index index(with size checking)
//! 
//! \param   index  Position of the element
//!
//! \return  Element at index 
//!
//! \throw   std::out_of_range
//!
//====================================================================================================================================

	virtual crVal_ operator[](size_t index) const;

//====================================================================================================================================
//!
//! \brief   Returnes number of elements in storage
//! 
//! \return  Storage size
//!
//====================================================================================================================================

	constexpr size_t size() const noexcept 
	{ 
		return SIZE; 
	}

//====================================================================================================================================
//!
//! \brief	 Swaps two instances of storage
//!
//! \param   rStorage  The storage to swap with
//!
//! \throw   maybe 
//!
//====================================================================================================================================

	void swap(Storage &rStorage) noexcept(std::_Is_nothrow_swappable<T>::value);

//====================================================================================================================================
//!
//! \brief	Recalculates the hash
//!
//====================================================================================================================================

	HASH_GUARD(inline void rehash() { hash_ = std::move(makeHash()); })

//====================================================================================================================================
//!
//! \brief	 Returns current hash
//!
//! \return  Hash
//!
//====================================================================================================================================

	HASH_GUARD(inline std::string_view getHash() const { return hash_; })

//====================================================================================================================================
//!
//! \brief	 Validates the storage condition
//!
//! \return  Is storage ok
//!
//====================================================================================================================================

	bool ok() const noexcept;

//====================================================================================================================================
//!
//! \brief	Dumps storage to the stream
//!
//! \param  rOstr  Stream to output
//!
//! \throw  smth
//!
//====================================================================================================================================

	// template<typename Char, typename Traits = std::char_traits<Char>>
	// void dump(std::basic_ostream<Char, Traits> &rOstr) const;

protected:
	CANARY_GUARD(const std::string CANARY_VALUE;)
	CANARY_GUARD(std::string canaryStart_;)
	
	std::array<T, SIZE> buf_;

	CANARY_GUARD(std::string canaryFinish_;)

//====================================================================================================================================
//!
//! \brief	 Constructs line to calculate hash
//!
//! \return  Hash
//!  
//! \throw   smth
//!
//====================================================================================================================================

	HASH_GUARD
	(
		std::string makeHash() const
		{
			std::string tmp;
			for (auto &&x : buf_) tmp += std::to_string(x);

			return std::string(NHash::Hash(tmp).getHash());
		}
	)

private:
	HASH_GUARD(std::string hash_;)

	static size_t numberOfInstances;
};

#pragma endregion 

#pragma region STATIC_VARIABLES

template<typename T, size_t SIZE>
size_t Storage<T, SIZE>::numberOfInstances = 0;

#pragma endregion

#pragma region FUNCTION_DECLARATION

template<typename T, size_t SIZE>
inline std::ostream &operator<<(std::ostream&, const Storage<T, SIZE>&);

#pragma endregion

#pragma region METHOD_DEFINITION

template<typename T, size_t SIZE>
Storage<T, SIZE>::Storage() noexcept :
	CANARY_GUARD(CANARY_VALUE(NHash::Hash("Storage" + ++numberOfInstances).getHash()), )
	CANARY_GUARD(canaryStart_(CANARY_VALUE), )
	HASH_GUARD(hash_(), )

	buf_()

	CANARY_GUARD(, canaryFinish_(CANARY_VALUE))
{		
	HASH_GUARD(hash_ = makeHash();)
	CANARY_GUARD(numberOfInstances--;)
	numberOfInstances++;

	GUARD_CHECK()
}

template<typename T, size_t SIZE>
Storage<T, SIZE>::Storage(const Storage &crStorage) noexcept :
	CANARY_GUARD(CANARY_VALUE(NHash::Hash("Storage" + ++numberOfInstances).getHash()), )
	CANARY_GUARD(canaryStart_(CANARY_VALUE), )
	HASH_GUARD(hash_(crStorage.hash_), )

	buf_(crStorage.buf_)

	CANARY_GUARD(, canaryFinish_(CANARY_VALUE))
{
	CANARY_GUARD(numberOfInstances--;)
	numberOfInstances++;

	GUARD_CHECK()
}

template<typename T, size_t SIZE>
Storage<T, SIZE>::Storage(Storage &&rrStorage) noexcept :
	CANARY_GUARD(CANARY_VALUE(NHash::Hash("Storage" + ++numberOfInstances).getHash()), )
	CANARY_GUARD(canaryStart_(CANARY_VALUE), )
	HASH_GUARD(hash_(std::move(rrStorage.hash_)), )

	buf_(std::move(rrStorage.buf_))

	CANARY_GUARD(, canaryFinish_(CANARY_VALUE))
{
	CANARY_GUARD(numberOfInstances--;)
	numberOfInstances++;

	rrStorage.buf_.fill(NULL);
	HASH_GUARD(rrStorage.hash_.clear();)
	
	GUARD_CHECK()
}

template<typename T, size_t SIZE>
inline Storage<T, SIZE>::~Storage()
{
	numberOfInstances--;

	GUARD_CHECK()
}

template<typename T, size_t SIZE>
inline Storage<T, SIZE> &Storage<T, SIZE>::operator=(const Storage &crStorage) noexcept
{
	GUARD_CHECK()

	if (this != &crStorage)
	{
		buf_ = crStorage.buf_;

		HASH_GUARD(rehash();)
	}

	GUARD_CHECK()

	return (*this);
}

template<typename T, size_t SIZE>
inline Storage<T, SIZE> &Storage<T, SIZE>::operator=(Storage &&rrStorage) noexcept
{
	GUARD_CHECK()

	assert(this != &rrStorage);

	buf_ = std::move(rrStorage.buf_);
	HASH_GUARD(hash_ = std::move(rrStorage.hash_);)

	rrStorage.buf_.fill(NULL);
	HASH_GUARD(rrStorage.hash_.clear();)

	GUARD_CHECK()

	return (*this);
}

template<typename T, size_t SIZE>
inline typename Storage<T, SIZE>::rVal_ Storage<T, SIZE>::operator[](size_t index)
{
	GUARD_CHECK()

	return  buf_.at(index);
}

template<typename T, size_t SIZE>
inline typename Storage<T, SIZE>::crVal_ Storage<T, SIZE>::operator[](size_t index) const
{
	GUARD_CHECK()

	return buf_.at(index);
}

template<typename T, size_t SIZE>
inline void Storage<T, SIZE>::swap(Storage &rStorage) noexcept(std::_Is_nothrow_swappable<T>::value)
{
	GUARD_CHECK()

	using std::swap; // To have all possible swaps

	buf_.swap(rStorage.buf_);
	HASH_GUARD(swap(hash_, rStorage.hash_);)

	GUARD_CHECK()
}

template<typename T, size_t SIZE>
bool Storage<T, SIZE>::ok() const noexcept
{
	return (CANARY_GUARD(canaryStart_ == CANARY_VALUE && canaryFinish_ == CANARY_VALUE && )
			HASH_GUARD(hash_ == makeHash() && )
			&buf_);
}

/*
template<typename T, size_t SIZE>
template<typename Char, typename Traits>
void Storage<T, SIZE>::dump(std::basic_ostream<Char, Traits> &rOstr) const
{
	rOstr << "[STORAGE DUMP]\n" 
          << "Storage <" << typeid(T).name() << ", " << SIZE << "> [0x" << this << "]\n{\n"
		  << "\tbuffer [" << SIZE << "] = 0x" << &buf_ << "\n\t{\n";

	for (size_t i = 0; i < SIZE; ++i) rOstr << "\t\t[" << std::setw(3) << i << "] =" << std::setw(8) << buf_[i] << std::endl;

	rOstr << "\n\t}\n\n";

	CANARY_GUARD
	(
		rOstr << "\tCANARY_VALUE  = " << CANARY_VALUE << std::endl;

		rOstr << "\tCANARY_START  = " << canaryStart_;
		if (canaryStart_ == CANARY_VALUE) NDebugger::Text(" TRUE ", rOstr, NDebugger::Colors::Green);
		else                              NDebugger::Text(" FALSE", rOstr, NDebugger::Colors::Red);

		rOstr << "\tCANARY_FINISH = " << canaryFinish_;
		if (canaryFinish_ == CANARY_VALUE) NDebugger::Text(" TRUE ", rOstr, NDebugger::Colors::Green);
		else                               NDebugger::Text(" FALSE", rOstr, NDebugger::Colors::Red);
	)

	HASH_GUARD
	(
		rOstr << "\n\tHASH = " << hash_;
		if (hash_ == makeHash()) NDebugger::Text(" TRUE ", rOstr, NDebugger::Colors::Green);
		else                     NDebugger::Text(" FALSE", rOstr, NDebugger::Colors::Red);
	)

	rOstr << "}\n[     END     ]\n\n";
}
*/

#pragma endregion

#pragma region FUNCTION_DEFINITION

template<typename T, size_t SIZE>
inline std::ostream& operator<<(std::ostream& rOstr, const Storage<T, SIZE> &crStorage)
{
	// crStorage.dump(rOstr);

	return rOstr;
}

#pragma endregion
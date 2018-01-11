#pragma once

#include <cassert> // assert
#include <iomanip> // std::setw
#include <array>   // std::array

#include "Guard.hpp"

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

	virtual rVal_  operator[](size_t);
	virtual crVal_ operator[](size_t) const;

	constexpr size_t size() const noexcept { return SIZE; }

	HASH_GUARD(inline void rehash() { hash_ = std::move(makeHash()); })

	HASH_GUARD(inline std::string_view getHash() const { return hash_; })

	void swap(Storage&) noexcept(std::_Is_nothrow_swappable<T>::value);

	virtual bool ok() const noexcept;
	virtual void dump(std::ostream& = std::cout) const;

protected:
	CANARY_GUARD(const std::string CANARY_VALUE;)
	CANARY_GUARD(std::string canaryStart_;)
	
	std::array<T, SIZE> buf_;

	CANARY_GUARD(std::string canaryFinish_;)

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

//====================================================================================================================================
//==========================================================STATIC_VARIABLES==========================================================
//====================================================================================================================================

template<typename T, size_t SIZE>
size_t Storage<T, SIZE>::numberOfInstances = 0;

//====================================================================================================================================
//========================================================FUNCTION_DECLARATION========================================================
//====================================================================================================================================

template<typename T, size_t SIZE>
inline std::ostream &operator<<(std::ostream&, const Storage<T, SIZE>&);

//====================================================================================================================================
//=========================================================METHOD_DEFINITION==========================================================
//====================================================================================================================================

#pragma region METHOD_DEFINITION

template<typename T, size_t SIZE>
inline Storage<T, SIZE>::Storage() _NOEXCEPT :
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
inline Storage<T, SIZE>::Storage(const Storage &crStorage) _NOEXCEPT :
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
inline Storage<T, SIZE>::Storage(Storage &&rrStorage) noexcept :
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

	if (index >= SIZE) throw std::out_of_range(std::string("[") + __FUNCTION__ + "] Storage out of range\n");

	GUARD_CHECK()

	return buf_[index];
}

template<typename T, size_t SIZE>
inline typename Storage<T, SIZE>::crVal_ Storage<T, SIZE>::operator[](size_t index) const
{
	GUARD_CHECK()

	if (index >= SIZE) throw std::out_of_range(std::string("[") + __FUNCTION__ + "] Storage out of range\n");

	GUARD_CHECK()

	return buf_[index];
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
inline bool Storage<T, SIZE>::ok() const _NOEXCEPT
{
	return (CANARY_GUARD(canaryStart_ == CANARY_VALUE && canaryFinish_ == CANARY_VALUE && )
			HASH_GUARD(hash_ == makeHash() && )
			&buf_);
}

template<typename T, size_t SIZE>
void Storage<T, SIZE>::dump(std::ostream &rOstr /* = std::cout */) const
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
		if (canaryStart_ == CANARY_VALUE) NDebugger::Info(" TRUE", NDebugger::TextColor::Green, true, rOstr);
		else                              NDebugger::Info(" FALSE", NDebugger::TextColor::Red,  true, rOstr);

		rOstr << "\tCANARY_FINISH = " << canaryFinish_;
		if (canaryFinish_ == CANARY_VALUE) NDebugger::Info(" TRUE", NDebugger::TextColor::Green, true, rOstr);
		else                               NDebugger::Info(" FALSE", NDebugger::TextColor::Red,  true, rOstr);
	)

	HASH_GUARD
	(
		rOstr << "\n\tHASH = " << hash_;
		if (hash_ == makeHash()) NDebugger::Info(" TRUE", NDebugger::TextColor::Green, true, rOstr);
		else                     NDebugger::Info(" FALSE", NDebugger::TextColor::Red,  true, rOstr);
	)

	rOstr << "}\n[     END     ]\n\n";
}

#pragma endregion

//====================================================================================================================================
//========================================================FUNCTION_DEFINITION=========================================================
//====================================================================================================================================

template<typename T, size_t SIZE>
inline std::ostream& operator<<(std::ostream& rOstr, const Storage<T, SIZE> &crStorage)
{
	crStorage.dump(rOstr);

	return rOstr;
}
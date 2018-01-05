#pragma once

#include <cassert> // assert
#include <iomanip> // std::setw
#include <array>   // std::array

#include "Guard.hpp"

template<typename T, SIZE_T SIZE>
class Storage 
{
public:
	typedef       T  &rVal_;
	typedef       T &&rrVal_;
	typedef CONST T  &crVal_;

	explicit Storage()      _NOEXCEPT;
	Storage(CONST Storage&) _NOEXCEPT;
	Storage(Storage&&)      _NOEXCEPT;
	~Storage();

	virtual Storage &operator=(CONST Storage&) _NOEXCEPT;
	virtual Storage &operator=(Storage&&)      _NOEXCEPT;

	virtual rVal_  operator[](SIZE_T);
	virtual crVal_ operator[](SIZE_T) const;

	_CONSTEXPR17 SIZE_T size() const _NOEXCEPT { return SIZE; }

	HASH_GUARD(inline VOID rehash() { hash_ = makeHash(); })

	HASH_GUARD(inline CRSTRING getHash() const { return hash_; })

	virtual VOID swap(Storage&) _NOEXCEPTARG(std::_Is_nothrow_swappable<T>());

	virtual BOOL ok() const _NOEXCEPT;
	virtual VOID dump(std::ostream& = std::cout) const;

protected:
	CANARY_GUARD(CONST std::string CANARY_VALUE;)
	CANARY_GUARD(std::string canaryStart_;)
	
	std::array<T, SIZE> buf_;

	CANARY_GUARD(std::string canaryFinish_;)

	HASH_GUARD
	(
		std::string makeHash() const
		{
			std::string tmp;
			for (CONST auto &x : buf_) tmp += std::to_string(x);

			return NHash::Hash(tmp).getHash();
		}
	)

private:
	HASH_GUARD(std::string hash_;)

	static SIZE_T numberOfInstances;
};

template<typename T, SIZE_T SIZE>
SIZE_T Storage<T, SIZE>::numberOfInstances = 0;

//====================================================================================================================================

template<typename T, SIZE_T SIZE>
inline std::ostream &operator<<(std::ostream&, CONST Storage<T, SIZE>&);

//====================================================================================================================================

template<typename T, SIZE_T SIZE>
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

template<typename T, SIZE_T SIZE>
inline Storage<T, SIZE>::Storage(CONST Storage &crStorage) _NOEXCEPT :
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

template<typename T, SIZE_T SIZE>
inline Storage<T, SIZE>::Storage(Storage &&rrStorage) _NOEXCEPT :
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

template<typename T, SIZE_T SIZE>
inline Storage<T, SIZE>::~Storage()
{
	numberOfInstances--;

	GUARD_CHECK()
}

template<typename T, SIZE_T SIZE>
inline Storage<T, SIZE> &Storage<T, SIZE>::operator=(CONST Storage &crStorage) _NOEXCEPT
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

template<typename T, SIZE_T SIZE>
inline Storage<T, SIZE> &Storage<T, SIZE>::operator=(Storage &&rrStorage) _NOEXCEPT
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

template<typename T, SIZE_T SIZE>
inline typename Storage<T, SIZE>::rVal_ Storage<T, SIZE>::operator[](SIZE_T index)
{
	GUARD_CHECK()

	if (index >= SIZE) throw std::out_of_range(__FUNCTION__);

	GUARD_CHECK()

	return buf_[index];
}

template<typename T, SIZE_T SIZE>
inline typename Storage<T, SIZE>::crVal_ Storage<T, SIZE>::operator[](SIZE_T index) const
{
	GUARD_CHECK()

	if (index >= SIZE) throw std::out_of_range(__FUNCTION__);

	GUARD_CHECK()

	return buf_[index];
}

template<typename T, SIZE_T SIZE>
inline VOID Storage<T, SIZE>::swap(Storage &rStorage) _NOEXCEPTARG(std::_Is_nothrow_swappable<T>())
{
	GUARD_CHECK()

	using std::swap; // To have all possible swaps

	swap(buf_, rStorage.buf_);
	HASH_GUARD(swap(hash_, rStorage.hash_);)

	GUARD_CHECK()
}

template<typename T, SIZE_T SIZE>
inline BOOL Storage<T, SIZE>::ok() const _NOEXCEPT
{
	return (CANARY_GUARD(canaryStart_ == CANARY_VALUE && canaryFinish_ == CANARY_VALUE && )
			HASH_GUARD(hash_ == makeHash() && )
			&buf_);
}

template<typename T, SIZE_T SIZE>
VOID Storage<T, SIZE>::dump(std::ostream &rOstr /* = std::cout */) const
{
	rOstr << "[STORAGE DUMP]\n" 
          << "Storage <" << typeid(T).name() << ", " << SIZE << "> [0x" << this << "]\n{\n"
		  << "\tbuffer [" << SIZE << "] = 0x" << &buf_ << "\n\t{\n";

	for (SIZE_T i = 0; i < SIZE; ++i) rOstr << "\t\t[" << std::setw(3) << i << "] =" << std::setw(8) << buf_[i] << std::endl;

	rOstr << "\n\t}\n\n";

	CANARY_GUARD
	(
		rOstr << "\tCANARY_VALUE  = " << CANARY_VALUE << std::endl;

		rOstr << "\tCANARY_START  = " << canaryStart_;
		if (canaryStart_ == CANARY_VALUE) NDebugger::Info(" TRUE", NDebugger::TextColor::Green, TRUE, rOstr);
		else                              NDebugger::Info(" FALSE", NDebugger::TextColor::Red,  TRUE, rOstr);

		rOstr << "\tCANARY_FINISH = " << canaryFinish_;
		if (canaryFinish_ == CANARY_VALUE) NDebugger::Info(" TRUE", NDebugger::TextColor::Green, TRUE, rOstr);
		else                               NDebugger::Info(" FALSE", NDebugger::TextColor::Red,  TRUE, rOstr);
	)

	HASH_GUARD
	(
		rOstr << "\n\tHASH = " << hash_;
		if (hash_ == makeHash()) NDebugger::Info(" TRUE", NDebugger::TextColor::Green, TRUE, rOstr);
		else                     NDebugger::Info(" FALSE", NDebugger::TextColor::Red,  TRUE, rOstr);
	)

	rOstr << "}\n[     END     ]\n\n";
}

//====================================================================================================================================

template<typename T, SIZE_T SIZE>
inline std::ostream& operator<<(std::ostream& rOstr, CONST Storage<T, SIZE> &crStorage)
{
	crStorage.dump(rOstr);

	return rOstr;
}
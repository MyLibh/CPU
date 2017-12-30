#pragma once

#include <array> // std::array

#include "MyTypedefs.hpp"
#include "Debugger.hpp"

namespace NRam
{
	template<typename T = INT>
	class RAM final
	{
	private:
		static CONST SIZE_T RAM_SIZE = 1 << 4;

	public:
		typedef       T  &rVal_;
		typedef       T &&rrVal_;
		typedef CONST T  &crVal_;

		explicit RAM()     _NOEXCEPT;
		RAM(CONST RAM<T>&) _NOEXCEPT;
		RAM(RAM<T>&&)      _NOEXCEPT;
		~RAM();

		RAM<T> &operator=(CONST RAM&) _NOEXCEPT;
		RAM<T> &operator=(RAM&&)      _NOEXCEPT;

		rVal_  operator[](SIZE_T);
		crVal_ operator[](SIZE_T) const;

		HASH_GUARD(inline VOID rehash() { hash_ = makeHash(); })

		SIZE_T put(crVal_);
		SIZE_T put(rrVal_);
		VOID pop();

		VOID swap(RAM&) _NOEXCEPTARG(std::_Is_nothrow_swappable<T>);

		BOOL ok() const _NOEXCEPT;
		VOID dump() const;

	private:
		CANARY_GUARD(CONST std::string CANARY_VALUE = NHash::Hash("RAM").getHash();)

		CANARY_GUARD(std::string canaryStart_;)
		HASH_GUARD(std::string hash_;)

		SIZE_T                  counter_;
		std::array<T, RAM_SIZE> buf_;

		CANARY_GUARD(std::string canaryFinish_;)

		HASH_GUARD
		(
			std::string makeHash() const
			{
				std::string tmp;
				tmp += std::to_string(counter_);
				for (SIZE_T i = 0; i < counter_; ++i) tmp += std::to_string(buf_[i]);

				return NHash::Hash(tmp).getHash();
			}
		)
	};

	template<typename T>
	inline RAM<T>::RAM() _NOEXCEPT :
		CANARY_GUARD(canaryStart_(CANARY_VALUE), )
		HASH_GUARD(hash_(), )

		counter_(NULL),
		buf_()

		CANARY_GUARD(, canaryFinish_(CANARY_VALUE))
	{ 
		HASH_GUARD(hash_ = makeHash();)

		GUARD_CHECK()
	}

	template<typename T>
	inline RAM<T>::RAM(CONST RAM &crRAM) _NOEXCEPT :
		CANARY_GUARD(canaryStart_(CANARY_VALUE), )
		HASH_GUARD(hash_(crRAM.hash_), )

		counter_(crRAM.counter_),
		buf_(crRAM.buf_)

		CANARY_GUARD(, canaryFinish_(CANARY_VALUE))
	{ 
		GUARD_CHECK()
	}
		
	template<typename T>
	inline RAM<T>::RAM(RAM &&rrRAM) _NOEXCEPT :
		CANARY_GUARD(canaryStart_(CANARY_VALUE), )
		HASH_GUARD(hash_(std::move(rrRAM.hash_)), )

		counter_(rrRAM.counter_),
		buf_(std::move(rrRAM.buf_))

		CANARY_GUARD(, canaryFinish_(CANARY_VALUE))
	{
		rrRAM.counter_ = NULL;
		rrRAM.buf_.fill(NULL); 
		HASH_GUARD(rrRAM.hash_.clear();)

		GUARD_CHECK()
	}

	template<typename T>
	inline RAM<T>::~RAM()
	{
		GUARD_CHECK()
	}

	template<typename T>
	inline RAM<T> &RAM<T>::operator=(CONST RAM &crRAM) _NOEXCEPT
	{
		GUARD_CHECK()

		if (this != &crRAM)
		{
			counter_ = crRAM.counter_;
			buf_     = crRAM.buf_;

			HASH_GUARD(hash_ = crRAM.hash_();)
		}

		GUARD_CHECK()

		return (*this);
	}

	template<typename T>
	inline RAM<T> &RAM<T>::operator=(RAM &&rrRAM) _NOEXCEPT
	{
		GUARD_CHECK()

		assert(this != &rrRAM);

		counter_ = rrRAM.counter_;
		buf_     = std::move(rrRAM.buf_);
		HASH_GUARD(hash_ = rrRAM.hash_;)

		rrRAM.counter_ = NULL;
		rrRAM.buf_.fill(NULL);
		HASH_GUARD(rrRAM.hash_.clear();)

		GUARD_CHECK()

		return (*this);
	}

	template<typename T>
	inline typename RAM<T>::rVal_ RAM<T>::operator[](SIZE_T index) 
	{
		GUARD_CHECK()

		if (index >= counter_) throw std::out_of_range("[RAM::operator[]] \"RAM out of range\"\n");

		GUARD_CHECK()

		return buf_[index];
	}

	template<typename T>
	inline typename RAM<T>::crVal_ RAM<T>::operator[](SIZE_T index) const
	{
		GUARD_CHECK()

		if (index >= counter_) throw std::out_of_range("[RAM::operator[]] \"RAM out of range\"\n");

		GUARD_CHECK()

		return buf_[index];
	}

	template<typename T>
	inline SIZE_T RAM<T>::put(crVal_ val)
	{
		GUARD_CHECK()

		if (counter_ == RAM_SIZE) throw std::out_of_range("[RAM::put] \"RAM out of range\"\n");

		buf_[counter_] = val;
		counter_++;
		HASH_GUARD(rehash();)

		GUARD_CHECK()

		return counter_ - 1;
	}

	template<typename T>
	inline SIZE_T RAM<T>::put(rrVal_ val)
	{
		GUARD_CHECK()

		if (counter_ == RAM_SIZE) throw std::out_of_range("[RAM::put] \"RAM out of range\"\n");

		buf_[counter_] = std::move(val);
		counter_++;
		HASH_GUARD(rehash();)

		GUARD_CHECK()

		return counter_ - 1;
	}

	template<typename T>
	inline VOID RAM<T>::pop()
	{
		GUARD_CHECK()

		if (!counter_) throw std::out_of_range("[RAM::put] \"RAM out of range\"\n");

		counter_--;
		HASH_GUARD(rehash();)

		GUARD_CHECK()
	}

	template<typename T>
	inline VOID swap(RAM<T> &rRAM) _NOEXCEPTARG(std::_Is_nothrow_swappable<T>)
	{
		GUARD_CHECK()
		
		using std::swap;

		swap(buf_, rRAM.buf_);
		HASH_GUARD(swap(hash_, rRAM.hash_);)

		GUARD_CHECK()
	}

	template<typename T>
	inline BOOL RAM<T>::ok() const _NOEXCEPT
	{
		return (this &&
				CANARY_GUARD(canaryStart_ == CANARY_VALUE && canaryFinish_ == CANARY_VALUE && )
				HASH_GUARD(hash_ == makeHash() && )
				(counter_ < RAM_SIZE) &&
				buf_.size());
	}

	template<typename T>
	VOID RAM<T>::dump() const
	{
		NDebugger::Info("\t[RAM DUMP]", NDebugger::TextColor::LightCyan);
		
		std::cout << "RAM <" << typeid(T).name() << "> [0x" << this << "]\n{\n"
			      << "\tram [" << counter_ << " of " << RAM_SIZE << "] = 0x" << &buf_ << "\n\t{\n\t\t";

		for (SIZE_T i = 0; i < counter_; ++i)
		{
			std::cout << "[" << std::setw(3) << i << "] = " << buf_[i] << (i + 1 == counter_ ? " " : ", ") << "  ";

			std::cout << "\n\t\t";
		}

		std::cout << "\n\t}\n";

		CANARY_GUARD
		(
			std::cout << "\tCANARY_VALUE  = " << CANARY_VALUE << std::endl;

			std::cout << "\tCANARY_START  = " << canaryStart_;
			if (canaryStart_ == CANARY_VALUE) NDebugger::Info(" TRUE",  NDebugger::TextColor::Green);
			else                              NDebugger::Info(" FALSE", NDebugger::TextColor::Red);

			std::cout << "\tCANARY_FINISH = " << canaryFinish_;
			if (canaryFinish_ == CANARY_VALUE) NDebugger::Info(" TRUE",  NDebugger::TextColor::Green);
			else                               NDebugger::Info(" FALSE", NDebugger::TextColor::Red);
		)

		HASH_GUARD
		(
			std::cout << "\n\tHASH = " << hash_;
			if (hash_ == makeHash()) NDebugger::Info(" TRUE",  NDebugger::TextColor::Green);
			else                     NDebugger::Info(" FALSE", NDebugger::TextColor::Red);
		)

			std::cout << "}\n";

		NDebugger::Info("\t[  END   ]\n", NDebugger::TextColor::LightCyan);
	}
} // namespace NRam
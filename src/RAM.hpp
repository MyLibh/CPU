#pragma once

#include <array> // std::array

#include "MyTypedefs.hpp"
#include "Debugger.hpp"

namespace NRam
{
	template<typename T = INT>
	struct RAM final
	{
	private:
		static CONST SIZE_T RAM_SIZE = 1 << 4;

		SIZE_T                  counter_;
		std::array<T, RAM_SIZE> buf_;
	public:
		

		typedef       T  &rVal_;
		typedef       T &&rrVal_;
		typedef CONST T  &crVal_;

		explicit RAM()     _NOEXCEPT;
		RAM(CONST RAM<T>&) _NOEXCEPT;
		RAM(RAM<T>&&)      _NOEXCEPT;
		~RAM();

		RAM<T> &operator=(CONST RAM&);
		RAM<T> &operator=(RAM&&);

		rVal_  operator[](SIZE_T);
		crVal_ operator[](SIZE_T) const;

		SIZE_T put(crVal_);
		SIZE_T put(rrVal_);
		VOID pop();

		VOID swap(RAM&) _NOEXCEPTARG(std::_Is_nothrow_swappable<T>);

		VOID dump() const;
	};

	template<typename T>
	inline RAM<T>::RAM() _NOEXCEPT :
		counter_(NULL),
		buf_()
	{ }

	template<typename T>
	inline RAM<T>::RAM(CONST RAM &crRAM) _NOEXCEPT :
		counter_(crRAM.counter_),
		buf_(crRAM.buf_)
	{ }
		
	template<typename T>
	inline RAM<T>::RAM(RAM &&rrRAM) _NOEXCEPT :
		counter_(rrRAM.counter_),
		buf_(std::move(rrRAM.buf_))
	{
		rrRAM.counter_ = NULL;
		// rrRAM.buf.fill(NULL); // QUEST: should be or not(then no noexcept)
	}

	template<typename T>
	inline RAM<T>::~RAM()
	{ }

	template<typename T>
	inline RAM<T> &RAM<T>::operator=(CONST RAM &crRAM)
	{
		if (this != &crRAM)
		{
			counter_ = crRAM.counter_;
			buf_     = crRAM.buf_;
		}

		return (*this);
	}

	template<typename T>
	inline RAM<T> &RAM<T>::operator=(RAM &&rrRAM)
	{
		assert(this != &rrRAM);

		counter_ = rrRAM.counter_;
		buf_     = std::move(rrRAM.buf_);

		rrRAM.counter_ = NULL;
		// rrRAM.buf.fill(NULL); // QUEST: should be or not(then no noexcept)

		return (*this);
	}

	template<typename T>
	inline typename RAM<T>::rVal_ RAM<T>::operator[](SIZE_T index) 
	{
		if (index >= counter_) throw std::out_of_range("[RAM::operator[]] \"RAM out of range\"\n");

		return buf_[index];
	}

	template<typename T>
	inline typename RAM<T>::crVal_ RAM<T>::operator[](SIZE_T index) const
	{
		if (index >= counter_) throw std::out_of_range("[RAM::operator[]] \"RAM out of range\"\n");

		return buf_[index];
	}

	template<typename T>
	inline SIZE_T RAM<T>::put(crVal_ val)
	{
		if (counter_ == RAM_SIZE) throw std::out_of_range("[RAM::put] \"RAM out of range\"\n");

		buf_[counter_] = val;
		counter_++;

		return counter_ - 1;
	}

	template<typename T>
	inline SIZE_T RAM<T>::put(rrVal_ val)
	{
		if (counter_ == RAM_SIZE) throw std::out_of_range("[RAM::put] \"RAM out of range\"\n");

		buf_[counter_] = std::move(val);
		counter_++;

		return counter_ - 1;
	}

	template<typename T>
	inline VOID RAM<T>::pop()
	{
		if (!counter_) throw std::out_of_range("[RAM::put] \"RAM out of range\"\n");

		counter_--;
	}

	template<typename T>
	inline VOID swap(RAM<T> &rRAM) _NOEXCEPTARG(std::_Is_nothrow_swappable<T>)
	{
		buf_.swap(rRAM.buf);
	}

	template<typename T>
	VOID RAM<T>::dump() const
	{
		NDebugger::Info("\t[RAM DUMP]", NDebugger::TextColors::LightCyan);
		
		std::cout << "RAM <" << typeid(T).name() << "> [0x" << this << "]\n{\n"
			      << "\tram [" << counter_ << " of " << RAM_SIZE << "] = 0x" << &buf_ << "\n\t{\n\t\t";

		for (SIZE_T i = 0; i < counter_; ++i)
		{
			std::cout << "[" << std::setw(3) << i << "] = " << buf_[i] << (i + 1 == counter_ ? " " : ", ") << "  ";

			std::cout << "\n\t\t";
		}

		std::cout << "\n\t}\n}\n";

		NDebugger::Info("\t[  END   ]\n", NDebugger::TextColors::LightCyan);
	}
} // namespace NRam
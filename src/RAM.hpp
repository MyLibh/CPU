#pragma once

#include "Debugger.hpp"
#include "Storage.hpp"
#include "Logger.hpp"

namespace NRam
{
	CONST SIZE_T RAM_SIZE = 1 << 4;

	template<typename T = INT>
	class Ram final : public Storage<T, RAM_SIZE>
	{
	public:
		explicit Ram()     _NOEXCEPT;
		Ram(CONST Ram<T>&) _NOEXCEPT;
		Ram(Ram<T>&&)      _NOEXCEPT;
		~Ram();

		Ram<T> &operator=(CONST Ram&) _NOEXCEPT;
		Ram<T> &operator=(Ram&&)      _NOEXCEPT ;

		virtual rVal_  operator[](SIZE_T)       override;
		virtual crVal_ operator[](SIZE_T) const override;

		SIZE_T put(crVal_);
		SIZE_T put(rrVal_);
		VOID pop();

		VOID swap(Ram&) _NOEXCEPTARG(std::_Is_nothrow_swappable<T>); //-V762

		virtual BOOL ok() const _NOEXCEPT override;
		virtual VOID dump(std::ostream& = std::cout) const override;

	private:
		SIZE_T counter_;
	};

	//====================================================================================================================================

	template<typename T>
	Logger& operator<<(Logger&, CONST Ram<T>&);

	//====================================================================================================================================

	template<typename T>
	inline Ram<T>::Ram() _NOEXCEPT :
		Storage(),
		counter_(NULL)
	{ 
		LOG_CONSTRUCTING()
	}

	template<typename T>
	inline Ram<T>::Ram(CONST Ram &crRam) _NOEXCEPT :
		Storage(crRam),
		counter_(crRam.counter_)
	{ 	
		LOG_CONSTRUCTING()
	}
		
	template<typename T>
	inline Ram<T>::Ram(Ram &&rrRam) _NOEXCEPT :
		Storage(rrRam),
		counter_(rrRam.counter_)
	{
		LOG_CONSTRUCTING()

		rrRam.counter_ = NULL;
	}

	template<typename T>
	inline Ram<T>::~Ram()
	{
		LOG_DESTRUCTING()
	}

	template<typename T>
	inline Ram<T> &Ram<T>::operator=(CONST Ram &crRam) _NOEXCEPT
	{
		if (this != &crRam)
		{
			*static_cast<Storage*>(this) = crRam;
			counter_                     = crRam.counter_;
		}

		return (*this);
	}

	template<typename T>
	inline Ram<T> &Ram<T>::operator=(Ram &&rrRam) _NOEXCEPT
	{
		assert(this != &rrRam);

		*static_cast<Storage*>(this) = std::move(rrRam);
		counter_                     = std::move(rrRam.counter_);

		rrRam.counter_ = NULL;

		return (*this);
	}
	
	template<typename T>
	inline typename Ram<T>::rVal_ Ram<T>::operator[](SIZE_T index)
	{
		if (index >= counter_) throw std::out_of_range(__FUNCTION__);

		return buf_[index];
	}

	template<typename T>
	inline typename Ram<T>::crVal_ Ram<T>::operator[](SIZE_T index) const
	{
		if (index >= counter_) throw std::out_of_range(__FUNCTION__);

		return buf_[index];
	}

	template<typename T>
	inline SIZE_T Ram<T>::put(crVal_ val)
	{
		if (counter_ == RAM_SIZE) throw std::out_of_range(__FUNCTION__);

		buf_[counter_] = val;
		counter_++;
		HASH_GUARD(rehash();)

		return counter_ - 1;
	}

	template<typename T>
	inline SIZE_T Ram<T>::put(rrVal_ val)
	{
		if (counter_ == RAM_SIZE) throw std::out_of_range(__FUNCTION__);

		buf_[counter_] = std::move(val);
		counter_++;
		HASH_GUARD(rehash();)

		return counter_ - 1;
	}

	template<typename T>
	inline VOID Ram<T>::pop()
	{
		if (!counter_) throw std::out_of_range(__FUNCTION__);

		counter_--;
		HASH_GUARD(rehash();)
	}

	template<typename T>
	inline VOID Ram<T>::swap(Ram<T> &rRam) _NOEXCEPTARG(std::_Is_nothrow_swappable<T>)
	{
		using std::swap; // To have all possible swaps

		swap(Storage, rRam);
		swap(counter_, rRam.counter_);
	}

	template<typename T>
	inline BOOL Ram<T>::ok() const _NOEXCEPT
	{
		return (Storage::ok() && (counter_ < RAM_SIZE));
	}

	template<typename T>
	VOID Ram<T>::dump(std::ostream &rOstr /* = std::cout */) const
	{
		NDebugger::Info("\t[RAM DUMP]", NDebugger::TextColor::LightCyan, TRUE, rOstr);
		
		rOstr << "Ram <" << typeid(T).name() << "> [0x" << this << "]\n{\n"
			  << "\tram [" << counter_ << " of " << RAM_SIZE << "] = 0x" << &buf_ << "\n\t{\n\t\t";

		for (SIZE_T i = 0; i < counter_; ++i)
		{
			rOstr << "[" << std::setw(3) << i << "] = " << buf_[i] << (i + 1 == counter_ ? " " : ", ") << "  ";

			rOstr << "\n\t\t";
		}

		rOstr << "\n\t}\n\n";

		CANARY_GUARD
		(
			rOstr << "\tCANARY_VALUE  = " << CANARY_VALUE << std::endl;

			rOstr << "\tCANARY_START  = " << canaryStart_;
			if (canaryStart_ == CANARY_VALUE) NDebugger::Info(" TRUE",  NDebugger::TextColor::Green, TRUE, rOstr);
			else                              NDebugger::Info(" FALSE", NDebugger::TextColor::Red,   TRUE, rOstr);

			rOstr << "\tCANARY_FINISH = " << canaryFinish_;
			if (canaryFinish_ == CANARY_VALUE) NDebugger::Info(" TRUE",  NDebugger::TextColor::Green, TRUE, rOstr);
			else                               NDebugger::Info(" FALSE", NDebugger::TextColor::Red,   TRUE, rOstr);
		)

		HASH_GUARD
		(
			rOstr << "\n\tHASH = " << getHash();
			if (getHash() == makeHash()) NDebugger::Info(" TRUE",  NDebugger::TextColor::Green, TRUE, rOstr);
			else                         NDebugger::Info(" FALSE", NDebugger::TextColor::Red,   TRUE, rOstr);
		)

		rOstr << "}\n";

		NDebugger::Info("\t[  END   ]\n", NDebugger::TextColor::LightCyan, TRUE, rOstr);
	}

	//====================================================================================================================================

	template<typename T>
	Logger& operator<<(Logger &rLogger, CONST Ram<T> &crRam)
	{
		std::string func("Ram<");
		func += typeid(T).name();
		func += ">";

		rLogger.stdPack(func);

		crRam.dump(rLogger.getOfstream());

		return rLogger;
	}
} // namespace NRam
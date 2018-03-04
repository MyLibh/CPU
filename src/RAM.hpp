#pragma once

#include "Storage.hpp"
#include "Logger.hpp"

namespace NRam
{

//====================================================================================================================================
//=============================================================CONSTANTS==============================================================
//====================================================================================================================================

	constexpr size_t RAM_SIZE = 1 << 4;

//====================================================================================================================================
//==============================================================CLASSES===============================================================
//====================================================================================================================================

	template<typename T = int>
	class Ram final : public Storage<T, RAM_SIZE>
	{
	public:
		explicit Ram()     noexcept;
		Ram(const Ram<T>&) noexcept;
		Ram(Ram<T>&&)      noexcept;
		~Ram();

		Ram<T> &operator=(const Ram&) noexcept;
		Ram<T> &operator=(Ram&&)      noexcept;

		virtual rVal_  operator[](size_t)       override;
		virtual crVal_ operator[](size_t) const override;

		size_t put(crVal_);
		size_t put(rrVal_);
		void pop();

		void swap(Ram&) noexcept(std::_Is_nothrow_swappable<T>::value); //-V762

		bool ok() const noexcept override;
		void dump(std::ostream& = std::cout) const;

	private:
		size_t counter_;
	};

//====================================================================================================================================
//========================================================FUNCTION_DECLARATION========================================================
//====================================================================================================================================

	template<typename T>
	Logger& operator<<(Logger&, const Ram<T>&);

//====================================================================================================================================
//=========================================================METHOD_DEFINITION==========================================================
//====================================================================================================================================

#pragma region METHOD_DEFINITION

	template<typename T>
	inline Ram<T>::Ram() noexcept :
		Storage(),
		counter_(NULL)
	{ 
		LOG_CONSTRUCTING()
	}

	template<typename T>
	inline Ram<T>::Ram(const Ram &crRam) noexcept :
		Storage(crRam),
		counter_(crRam.counter_)
	{ 	
		LOG_CONSTRUCTING()
	}
		
	template<typename T>
	inline Ram<T>::Ram(Ram &&rrRam) noexcept :
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
	inline Ram<T> &Ram<T>::operator=(const Ram &crRam) noexcept
	{
		if (this != &crRam)
		{
			*static_cast<Storage*>(this) = crRam;
			counter_                     = crRam.counter_;
		}

		return (*this);
	}

	template<typename T>
	inline Ram<T> &Ram<T>::operator=(Ram &&rrRam) noexcept
	{
		assert(this != &rrRam);

		*static_cast<Storage*>(this) = std::move(rrRam);
		counter_                     = std::move(rrRam.counter_);

		rrRam.counter_ = NULL;

		return (*this);
	}
	
	template<typename T>
	inline typename Ram<T>::rVal_ Ram<T>::operator[](size_t index)
	{
		if (index >= counter_) throw std::out_of_range(std::string("[") + __FUNCTION__ + "] Ram out of range\n");

		return buf_[index];
	}

	template<typename T>
	inline typename Ram<T>::crVal_ Ram<T>::operator[](size_t index) const
	{
		if (index >= counter_) throw std::out_of_range(std::string("[") + __FUNCTION__ + "] Ram out of range\n");

		return buf_[index];
	}

	template<typename T>
	inline size_t Ram<T>::put(crVal_ val)
	{
		if (counter_ == RAM_SIZE) throw std::length_error(std::string("[") + __FUNCTION__ + "] Ram length error\n");

		buf_[counter_] = val;
		counter_++;
		HASH_GUARD(rehash();)

		return counter_ - 1;
	}

	template<typename T>
	inline size_t Ram<T>::put(rrVal_ val)
	{
		if (counter_ == RAM_SIZE) throw std::length_error(std::string("[") + __FUNCTION__ + "] Ram length error\n");

		buf_[counter_] = std::move(val);
		counter_++;
		HASH_GUARD(rehash();)

		return counter_ - 1;
	}

	template<typename T>
	inline void Ram<T>::pop()
	{
		if (!counter_) throw std::length_error(std::string("[") + __FUNCTION__ + "] Ram length error\n");

		counter_--;
		HASH_GUARD(rehash();)
	}

	template<typename T>
	inline void Ram<T>::swap(Ram<T> &rRam) noexcept(std::_Is_nothrow_swappable<T>::value)
	{
		using std::swap; // To have all possible swaps

		Storage::swap(rRam);
		swap(counter_, rRam.counter_);
	}

	template<typename T>
	inline bool Ram<T>::ok() const noexcept
	{
		return (Storage::ok() && (counter_ < RAM_SIZE));
	}

	template<typename T>
	void Ram<T>::dump(std::ostream &rOstr /* = std::cout */) const
	{
		NDebugger::Text("\t[RAM DUMP]", rOstr, NDebugger::Colors::LightCyan);
		
		rOstr << "Ram <" << typeid(T).name() << "> [0x" << this << "]\n{\n"
			  << "\tram [" << counter_ << " of " << RAM_SIZE << "] = 0x" << &buf_ << "\n\t{\n\t\t";

		for (size_t i = 0; i < counter_; ++i)
		{
			rOstr << "[" << std::setw(3) << i << "] = " << buf_[i] << (i + 1 == counter_ ? " " : ", ") << "  ";

			rOstr << "\n\t\t";
		}

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
			rOstr << "\n\tHASH = " << getHash().data();
			if (getHash() == makeHash()) NDebugger::Text(" TRUE ", rOstr, NDebugger::Colors::Green);
			else                         NDebugger::Text(" FALSE", rOstr, NDebugger::Colors::Red);
		)

		rOstr << "}\n";

		NDebugger::Text("\t[  END   ]\n", rOstr, NDebugger::Colors::LightCyan);
	}

#pragma endregion

//====================================================================================================================================
//========================================================FUNCTION_DEFINITION=========================================================
//====================================================================================================================================

	template<typename T>
	Logger& operator<<(Logger &rLogger, const Ram<T> &crRam)
	{
		std::string_view func("Ram<" + typeid(T).name() + ">");

		rLogger.stdPack(func);

		crRam.dump(rLogger.getOfstream());

		return rLogger;
	}

} // namespace NRam
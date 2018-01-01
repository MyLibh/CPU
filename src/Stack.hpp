#pragma once

#include <memory>  // std::unique_ptr
#include <cassert> // assert
#include <iomanip> // std::setw

#include "Debugger.hpp"
#include "Guard.hpp"
#include "Logger.hpp"

//cmd -D_SCL_SECURE_NO_WARNINGS

extern Logger gLogger;

namespace NStack
{
	template<typename T = INT>
	class Stack final
	{
		VOID reallocMemory();

	public:
		static CONST SIZE_T DEFAULT_SIZE = 2;

		typedef       T &&rrVal_;
		typedef CONST T  &crVal_;

		explicit Stack(SIZE_T = DEFAULT_SIZE) _NOEXCEPT;
		Stack(CONST Stack&); // QUEST: maybe i can make it noexcept
		Stack(Stack&&)                        _NOEXCEPT;
		~Stack();

		Stack<T> &operator=(CONST Stack&); // QUEST: maybe i can make it noexcept
		Stack<T> &operator=(Stack&&) _NOEXCEPT;

		BOOL operator==(CONST Stack&) const;
		BOOL operator!=(CONST Stack&) const;

		SIZE_T size()  const _NOEXCEPT;
		BOOL   empty() const _NOEXCEPT;

		VOID push(crVal_);
		VOID push(rrVal_);
		VOID pop();

		crVal_ top() const;

		VOID swap(Stack&) _NOEXCEPTARG(std::_Is_nothrow_swappable<T>);

		HASH_GUARD(inline SIZE_T getHash() const _NOEXCEPT { return hash_; })

		BOOL ok() const _NOEXCEPT;
		VOID dump(std::ostream& = std::cout) const;

	private:
		CANARY_GUARD(CONST std::string CANARY_VALUE;)

		CANARY_GUARD(std::string canaryStart_;)
		HASH_GUARD(std::string hash_;)

		SIZE_T               counter_,
		size_;
		std::unique_ptr<T[]> buffer_;

		CANARY_GUARD(std::string canaryFinish_;)

		HASH_GUARD
		(
			std::string makeHash() const
			{
				std::string tmp;
				tmp += std::to_string(counter_);
				tmp += std::to_string(size_);

				for (SIZE_T i = 0; i < counter_; ++i) tmp += std::to_string(buffer_[i]);

				return NHash::Hash(tmp).getHash();				
			}
		)

		static SIZE_T numberOfInstances;
	};

	template<typename T>
	SIZE_T Stack<T>::numberOfInstances = 0;

	//====================================================================================================================================

	template<typename T>
	Logger& operator<<(Logger&, CONST Stack<T>&);

	//====================================================================================================================================

	template<typename T>
	inline Stack<T>::Stack(SIZE_T size /* = DEFAULT_SIZE */) _NOEXCEPT :
		CANARY_GUARD(CANARY_VALUE(NHash::Hash("Stack" + ++numberOfInstances).getHash()),)
		CANARY_GUARD(canaryStart_(CANARY_VALUE),)
		HASH_GUARD(hash_(),)

		counter_(NULL),
		size_(size),
		buffer_(new T[size])

		CANARY_GUARD(, canaryFinish_(CANARY_VALUE))
	{
		HASH_GUARD(hash_ = makeHash();)

		CANARY_GUARD(numberOfInstances--;)
		numberOfInstances++;

		LOG_CONSTRUCTING()

		GUARD_CHECK()
	}

	template<typename T>
	inline Stack<T>::Stack(CONST Stack &crStack) :
		CANARY_GUARD(CANARY_VALUE(NHash::Hash("Stack" + ++numberOfInstances).getHash()),)
		CANARY_GUARD(canaryStart_(CANARY_VALUE),)
		HASH_GUARD(hash_(),)

		counter_(crStack.counter_),
		size_(crStack.size_),
		buffer_(new T[crStack.size_])

		CANARY_GUARD(, canaryFinish_(CANARY_VALUE))
	{
		std::copy(crStack.buffer_.get(), crStack.buffer_.get() + crStack.counter_, buffer_.get());

		HASH_GUARD(hash_ = makeHash();)

		CANARY_GUARD(numberOfInstances--;)
		numberOfInstances++;

		LOG_CONSTRUCTING()

		GUARD_CHECK()
	}

	template<typename T>
	inline Stack<T>::Stack(Stack &&rrStack) _NOEXCEPT :
		CANARY_GUARD(CANARY_VALUE(NHash::Hash("Stack" + ++numberOfInstances).getHash()),)
		CANARY_GUARD(canaryStart_(CANARY_VALUE), )
		HASH_GUARD(hash_(rrStack.hash_), )

		counter_(rrStack.counter_),
		size_(rrStack.size_),
		buffer_(rrStack.buffer_)

		CANARY_GUARD(, canaryFinish_(CANARY_VALUE))
	{
		rrStack.counter_ = NULL;
		rrStack.size_    = NULL;
		rrStack.buffer_  = nullptr;
		HASH_GUARD(rrStack.hash_.clear();)

		CANARY_GUARD(numberOfInstances--;)
		numberOfInstances++;

		LOG_CONSTRUCTING()

		GUARD_CHECK()
	}

	template<typename T>
	inline Stack<T>::~Stack()
	{
		numberOfInstances--;

		LOG_DESTRUCTING()

		GUARD_CHECK()
	}

	template<typename T>
	VOID Stack<T>::reallocMemory()
	{
		GUARD_CHECK()

		T *pBuf = buffer_.release();

		size_ <<= 1;
		buffer_.reset(new T[size_]);

		std::copy(pBuf, pBuf + counter_, buffer_.get());

		HASH_GUARD(hash_ = makeHash();)

		GUARD_CHECK()
	}

	template<typename T>
	Stack<T> &Stack<T>::operator=(CONST Stack &crStack)
	{
		GUARD_CHECK()

		if (this != &crStack)
		{
			counter_ = crStack.counter_;

			if (size_ != crStack.size_)
			{
				size_ = crStack.size_;

				buffer_.reset(new T[size_]);
			}

			std::copy(crStack.buffer_.get(), crStack.buffer_.get() + crStack.size_, buffer_.get());
		}
		HASH_GUARD(hash_ = crStack.hash_;)

		GUARD_CHECK()

		return (*this);
	}

	template<typename T>
	Stack<T> &Stack<T>::operator=(Stack &&rrStack) _NOEXCEPT
	{
		GUARD_CHECK()

		assert(this != &rrStack);

		buffer_ = nullptr;

		counter_ = rrStack.counter_;
		size_    = rrStack.size_;
		buffer_  = std::move(rrStack.buffer_);

		HASH_GUARD(hash_ = rrStack.hash_;)

		rrStack.counter_ = NULL;
		rrStack.size_    = NULL;
		rrStack.buffer_  = nullptr;
		HASH_GUARD(rrStack.hash_.clear();)

		GUARD_CHECK()

		return (*this);
	}

	template<typename T>
	inline BOOL Stack<T>::operator==(CONST Stack &crStack) const
	{
		GUARD_CHECK()

		if (counter_ != crStack.counter_) return FALSE;

		if (!counter_) return TRUE; // Zero-size verification

		GUARD_CHECK()

		return std::equal(&buffer_[0], &buffer_[counter_ - 1], &crStack.buffer_[0]);
	}

	template<typename T>
	inline BOOL Stack<T>::operator!=(CONST Stack &crStack) const 
	{
		return (!(*this == crStack));
	}

	template<typename T>
	inline SIZE_T Stack<T>::size() const _NOEXCEPT
	{ 
		return counter_;
	}

	template<typename T>
	inline BOOL Stack<T>::empty() const _NOEXCEPT
	{ 
		return (counter_ ? FALSE : TRUE); 
	}

	template<typename T>
	inline VOID Stack<T>::push(crVal_ val)
	{
		GUARD_CHECK()

		if (counter_ + 1 == size_) reallocMemory();

		buffer_[counter_] = val;
		++counter_;

		HASH_GUARD(hash_ = makeHash();)

		GUARD_CHECK()
	}

	template<typename T>
	inline VOID Stack<T>::push(rrVal_ val)
	{
		GUARD_CHECK()

		if (counter_ + 1 == size_) reallocMemory();

		buffer_[counter_] = val;
		++counter_;

		HASH_GUARD(hash_ = makeHash();)

		GUARD_CHECK()
	}

	template<typename T>
	inline VOID Stack<T>::pop()
	{
		GUARD_CHECK()

		if (!counter_) throw std::out_of_range("[Stack::pop] \"Stack out of range\"\n");

		--counter_;

		HASH_GUARD(hash_ = makeHash();)

		GUARD_CHECK()
	}

	template<typename T>
	inline typename Stack<T>::crVal_ Stack<T>::top() const
	{
		GUARD_CHECK()

		if (!counter_) std::out_of_range("[Stack::pop] \"Stack out of range\"\n");

		GUARD_CHECK()

		return buffer_[counter_ - 1];
	}

	template<typename T>
	VOID Stack<T>::swap(Stack &rStack) _NOEXCEPTARG(std::_Is_nothrow_swappable<T>)
	{
		GUARD_CHECK()

		using std::swap; // To have all possible swaps

		HASH_GUARD(swap(hash_, rStack.hash_);)

		swap(counter_, rStack.counter_);
		swap(size_, rStack.size_);

		swap(buffer_, rStack.buffer_);

		GUARD_CHECK()
	}

	template<typename T>
	inline BOOL Stack<T>::ok() const _NOEXCEPT
	{
		return (this &&
				CANARY_GUARD(canaryStart_ == CANARY_VALUE && canaryFinish_ == CANARY_VALUE && )
				HASH_GUARD(hash_ == makeHash() && )
				(size_ > counter_) && buffer_);		
	}

	template<typename T>
	VOID Stack<T>::dump(std::ostream &rOstr /* = std::cout */) const
	{
		NDebugger::Info("\t[STACK DUMP]", NDebugger::TextColor::Yellow, TRUE, rOstr);

		rOstr << "Stack <" << typeid(T).name() << "> [0x" << this << "]\n"
			  << "{\n\tbuffer [" << counter_ << "] = 0x" << buffer_.get() << "\n\t{\n";

		if (counter_) for (SIZE_T i = 0; i < counter_; ++i) rOstr << "\t\t[" << i << "] = " << std::setw(8) << buffer_[i] << std::endl;
		else                                                rOstr << "\t\tempty\n";

		rOstr << "\t}\n\n";

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
			rOstr << "\n\tHASH = " << hash_;		
			if (hash_ == makeHash()) NDebugger::Info(" TRUE",  NDebugger::TextColor::Green, TRUE, rOstr);
			else                     NDebugger::Info(" FALSE", NDebugger::TextColor::Red,   TRUE, rOstr);
		)

		std::cout << "}\n";

		NDebugger::Info("\t[   END    ]\n", NDebugger::TextColor::Yellow, TRUE, rOstr);
	}

	//====================================================================================================================================

	template<typename T>
	Logger& operator<<(Logger &rLogger, CONST Stack<T> &crStack)
	{
		std::string func("Stack<");
		func += typeid(T).name();
		func += ">";

		rLogger.stdPack(func);

		crStack.dump(rLogger.getOfstream());

		return rLogger;
	}
} // namespace NStack
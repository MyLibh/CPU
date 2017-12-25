#pragma once

#include <memory>  // std::unique_ptr
#include <cassert> // assert
#include <iomanip> // std::setw

#include "Debugger.hpp"

//cmd -D_SCL_SECURE_NO_WARNINGS

namespace NStack
{
	template<typename T = INT>
	class Stack final
	{
		static CONST SIZE_T DEFAULT_SIZE = 10;

		SIZE_T               counter_,
			                 size_;
		std::unique_ptr<T[]> buffer_;

		VOID reallocMemory();

	public:
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
		
		VOID dump() const;
	};

	template<typename T>
	inline Stack<T>::Stack(SIZE_T size /* = 10 */) _NOEXCEPT :
		counter_(NULL),
		size_(size),
		buffer_(new T[size])
	{ }

	template<typename T>
	inline Stack<T>::Stack(CONST Stack &crStack) :
		counter_(crStack.counter_),
		size_(crStack.size_),
		buffer_(new T[crStack.size_])
	{
		std::copy(crStack.buffer_.get(), crStack.buffer_.get() + crStack.counter_, buffer_.get());
	}

	template<typename T>
	inline Stack<T>::Stack(Stack &&rrStack) _NOEXCEPT :
		counter_(rrStack.counter_),
		size_(rrStack.size_),
		buffer_(rrStack.buffer_)
	{
		rrStack.counter_ = NULL;
		rrStack.size_    = NULL;
		rrStack.buffer_  = nullptr;
	}

	template<typename T>
	inline Stack<T>::~Stack()
	{ }

	template<typename T>
	VOID Stack<T>::reallocMemory()
	{
		T *pBuf = buffer_.release();

		size_ <<= 1;
		buffer_.reset(new T[size_]);

		std::copy(pBuf, pBuf + counter_, buffer_.get());
	}

	template<typename T>
	Stack<T> &Stack<T>::operator=(CONST Stack &crStack)
	{
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

		return (*this);
	}

	template<typename T>
	Stack<T> &Stack<T>::operator=(Stack &&rrStack) _NOEXCEPT
	{
		assert(this != &rrStack);

		buffer_ = nullptr;

		counter_ = rrStack.counter_;
		size_    = rrStack.size_;
		buffer_  = std::move(rrStack.buffer_);

		rrStack.counter_ = NULL;
		rrStack.size_    = NULL;
		rrStack.buffer_  = nullptr;

		return (*this);
	}

	template<typename T>
	inline BOOL Stack<T>::operator==(CONST Stack &crStack) const
	{
		if (counter_ != crStack.counter_) return FALSE;

		if (!counter_) return TRUE; // Zero-size verification

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
		if (counter_ + 1 == size_) reallocMemory();

		buffer_[counter_] = val;
		++counter_;
	}

	template<typename T>
	inline VOID Stack<T>::push(rrVal_ val)
	{
		if (counter_ + 1 == size_) reallocMemory();

		buffer_[counter_] = val;
		++counter_;
	}

	template<typename T>
	inline VOID Stack<T>::pop()
	{
		if (!counter_) throw std::out_of_range("[Stack::pop] \"Stack out of range\"\n");

		--counter_;
	}

	template<typename T>
	inline typename Stack<T>::crVal_ Stack<T>::top() const
	{
		if (!counter_) std::out_of_range("[Stack::pop] \"Stack out of range\"\n");

		return buffer_[counter_ - 1];
	}

	template<typename T>
	VOID Stack<T>::swap(Stack<T> &rStack) _NOEXCEPTARG(std::_Is_nothrow_swappable<T>)
	{
		std::swap(counter_, rStack.counter_);
		std::swap(size_, rStack.size_);

		buffer_.swap(rStack.buffer_);
	}

	template<typename T>
	VOID Stack<T>::dump() const
	{
		NDebugger::Info("\t[STACK DUMP]", NDebugger::TextColors::Yellow);

		std::cout << "Stack <" << typeid(T).name() << "> [0x" << this << "]\n"
			<< "{\n\tbuffer [" << counter_ << "] = 0x" << buffer_.get() << "\n\t{\n";

		if (counter_) for (SIZE_T i = 0; i < counter_; ++i) std::cout << "\t\t[" << i << "] = " << std::setw(8) << buffer_[i] << std::endl;
		else                                                std::cout << "\t\tempty\n";

		std::cout << "\t}\n}\n";

		NDebugger::Info("\t[   END    ]\n", NDebugger::TextColors::Yellow);
	}
} // namespace NStack
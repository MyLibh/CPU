#pragma once

#include <vector>
#include <iostream>
#include <memory>
#include <iomanip>

#include "MyTypedefs.hpp"
#include "Debugger.hpp"

//cmd -D_SCL_SECURE_NO_WARNINGS

namespace NStack
{
	template<typename T = INT>
	class Stack final
	{
		SIZE_T               counter_,
			                 size_;
		std::unique_ptr<T[]> buffer_;

		VOID ReallocMemory();

	public:
		typedef       T &&rrVal_;
		typedef CONST T  &crVal_;

		explicit Stack(SIZE_T = 10);
		Stack(CONST Stack<T>&);
		Stack(Stack<T>&&);
		~Stack();

		Stack<T> &operator=(CONST Stack<T>&);
		Stack<T> &operator=(Stack<T>&&);

		BOOL operator==(CONST Stack<T>&) const;
		BOOL operator!=(CONST Stack<T>&) const;

		SIZE_T size() const;
		BOOL empty() const;

		VOID push(crVal_);
		VOID push(rrVal_);
		VOID pop();

		crVal_ top() const;

		VOID swap(Stack<T>&);
		
		VOID dump() const;

		class OutOfRangeExc : std::exception
		{
		public:
			OutOfRangeExc(CRSTRING error = "Stack is empty\n") : exception(error.c_str()) { }
		};
	};

	template<typename T>
	Stack<T>::Stack(SIZE_T size /* = 10 */) :
		counter_(NULL),
		size_(size),
		buffer_(new T[size])
	{ }

	template<typename T>
	Stack<T>::Stack(CONST Stack<T> &crStack) :
		counter_(crStack.counter_),
		size_(crStack.size_),
		buffer_(new T[crStack.size_])
	{
		std::copy(crStack.buffer_.get(), crStack.buffer_.get() + crStack.size_, buffer_.get());
	}

	template<typename T>
	Stack<T>::Stack(Stack<T> &&rrStack) :
		counter_(rrStack.counter_),
		size_(rrStack.size_),
		buffer_(std::move(rrStack.buffer_))
	{
		rrStack.counter_ = NULL;
		rrStack.size_ = NULL;
		rrStack.buffer_ = nullptr;
	}

	template<typename T>
	Stack<T>::~Stack()
	{ }

	template<typename T>
	VOID Stack<T>::ReallocMemory()
	{
		T      *pBuf = buffer_.release();
		SIZE_T  tmp = size_;

		size_ <<= 1;
		buffer_.reset(new T[size_]);

		std::copy(pBuf, pBuf + tmp, buffer_.get());
	}

	template<typename T>
	Stack<T> &Stack<T>::operator=(CONST Stack<T> &crStack)
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
	Stack<T> &Stack<T>::operator=(Stack<T> &&rrStack)
	{
		assert(this != &rrStack);

		buffer_ = nullptr;

		counter_ = rrStack.counter_;
		size_ = rrStack.size_;
		buffer_ = std::move(rrStack.buffer_);

		rrStack.counter_ = NULL;
		rrStack.size_ = NULL;
		rrStack.buffer_ = nullptr;

		return (*this);
	}
	
	template<typename T>
	BOOL Stack<T>::operator==(CONST Stack<T> &crStack) const
	{
		if (counter_ != crStack.counter_) return FALSE;

		for (SIZE_T i = 0; i < counter_; ++i)
			if (buffer_[i] != crStack.buffer_[i]) return FALSE;

		return TRUE;
	}

	template<typename T>
	BOOL Stack<T>::operator!=(CONST Stack<T> &crStack) const
	{
		return !(*this == crStack);
	}

	template<typename T>
	SIZE_T Stack<T>::size() const 
	{ 
		return counter_;
	}

	template<typename T>
	BOOL Stack<T>::empty() const 
	{ 
		return (counter ? FALSE : TRUE); 
	}

	template<typename T>
	VOID Stack<T>::push(crVal_ val)
	{
		if (++counter_ == size_) ReallocMemory();

		buffer_[counter_ - 1] = val;
	}

	template<typename T>
	VOID Stack<T>::push(rrVal_ val)
	{
		if (++counter_ == size_) ReallocMemory();

		buffer_[counter_ - 1] = val;
	}

	template<typename T>
	VOID Stack<T>::pop()
	{
		if (counter_) counter_--;
		else throw OutOfRangeExc();
	}

	template<typename T>
	CONST T &Stack<T>::top() const
	{
		if (counter_) return buffer_[counter_ - 1];
		else throw OutOfRangeExc();
	}

	template<typename T>
	VOID Stack<T>::swap(Stack<T> &rStack)
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

		if (counter_) for (SIZE_T i = NULL; i < counter_; ++i) std::cout << "\t\t[" << i << "] = " << std::setw(8) << buffer_[i] << std::endl;
		else                                                   std::cout << "\t\tempty\n";

		std::cout << "\t}\n}\n";

		NDebugger::Info("\t[   END    ]\n", NDebugger::TextColors::Yellow);
	}
};
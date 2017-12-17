#pragma once

#include <vector>
#include <iostream>
#include <memory>

#include "MyTypedefs.hpp"
#include "Debugger.hpp"

//cmd -D_SCL_SECURE_NO_WARNINGS

template<typename T = INT>
class Stack final
{ 
    SIZE_T               counter_,
		   	             size_;
    std::unique_ptr<T[]> buffer_;

public:
    explicit Stack(SIZE_T = 10);
	Stack(CONST Stack<T>&);
	Stack(Stack<T>&&);
    ~Stack() { }

	Stack<T> &operator=(CONST Stack<T>&);
	Stack<T> &operator=(Stack<T>&&);

	SIZE_T size() const { return counter_; } 
	BOOL empty() const { return counter? FALSE : TRUE; }
    
	VOID push(const T&);
	VOID push(T&&);
	VOID pop();
	
	CONST T &top() const;
    
	VOID swap(Stack<T>&);
	// TODO: add opertors ==, !=
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
	rrStack.size_    = NULL;
	rrStack.buffer_  = nullptr;
}

template<typename T>
Stack<T> &Stack<T>::operator=(CONST Stack<T> &crStack)
{
	if(this != &crStack)
	{
		counter_ = crStack.counter_;

		if(size_ != crStack.size_)
		{
			size_ = crStack.size_;

			buffer_.reset(new T[size_]);
		}

		std::copy(crStack.buffer_.get(), crStack.buffer_.get() + crStack.size_, buffer_.get());
	}

	return *this;
}

template<typename T>
Stack<T> &Stack<T>::operator=(Stack<T> &&rrStack)
{
	assert(this != &rrStack);

	buffer_  = nullptr;

	counter_ = rrStack.counter_;
	size_    = rrStack.size_;
	buffer_  = std::move(rrStack.buffer_);
	
	rrStack.counter_ = NULL;
	rrStack.size_    = NULL;
	rrStack.buffer_  = nullptr;

	return *this;
}

template<typename T>
VOID Stack<T>::push(CONST T &crVal)
{
    if(++counter_ == size_) 
	{
		T      *pBuf = buffer_.release();
		SIZE_T  tmp  = size_;

		size_ <<= 1;
		buffer_.reset(new T[size_]);
 
		std::copy(pBuf, pBuf + tmp, buffer_.get());
	}
	
	buffer_[counter_ - 1] = crVal;
}

template<typename T>
VOID Stack<T>::push(T &&rrVal)
{
    if(++counter_ == size_) 
	{
		T      *pBuf = buffer_.release();
		SIZE_T  tmp  = size_;

		size_ <<= 1;
		buffer_.reset(new T[size_]);
 
		std::copy(pBuf, pBuf + tmp, buffer_.get());
	}
	
	buffer_[counter_ - 1] = rrVal;
}

template<typename T>
VOID Stack<T>::pop()
{
	if(counter_) counter_--;
	else throw OutOfRangeExc();
}

template<typename T>
CONST T &Stack<T>::top() const
{
	if(counter_) return buffer_[counter_ - 1];
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
	Debugger::Info("[STACK DUMP]", Debugger::TextColors::Yellow);

    if(counter_) for(SIZE_T i = NULL; i < counter_; ++i) std::cout << buffer_[i] << " ";
	else                                                 std::cout << "empty";

    std::cout << std::endl;
}
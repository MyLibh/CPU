#pragma once

#include <vector>
#include <iostream>
#include <memory>

#include "MyTypedefs.hpp"

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

	inline SIZE_T size() const { return counter_; } 
	inline BOOL empty() const { return counter? FALSE : TRUE; }
    
	VOID push(const T&);
    T pop();
	VOID swap(Stack<T>&);

    VOID dump() const;
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
void Stack<T>::push(const T &crVal)
{
    if(++counter_ == size_) 
	{
		T      *pBuf = buffer_.release();
		SIZE_T  tmp  = size_;

		size_ <<= 1;
		buffer_.reset(new T[size_]);
 
		std::copy(pBuf, pBuf + tmp, buffer_.get());
	}
	else buffer_[counter_ - 1] = crVal;
}

template<typename T>
inline T Stack<T>::pop()
{
    return buffer_[counter_ ? --counter_ : counter_];
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
    if(counter_) for(SIZE_T i = NULL; i < counter_; ++i) std::cout << buffer_[i] << " ";
	else std::cout << "empty";

    std::cout << std::endl;
}
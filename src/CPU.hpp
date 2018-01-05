#pragma once

#include "Register.hpp"
#include "RAM.hpp"
#include "Stack.hpp"
#include "MyMath.hpp"

namespace NCpu
{
#pragma region Usings

	using NRegister::REG;
	using NRegister::Register;

	using NRam::Ram;

	using NStack::Stack; 

#pragma endregion

	template<typename T = INT>
	class CPU final
	{
	public:
		typedef       T  &rVal_;
		typedef       T &&rrVal_;
		typedef CONST T  &crVal_;

		enum class MemoryStorage
		{
			REGISTER,
			RAM,
			STACK,
			STACK_FUNC_RET_ADDR
		};

		explicit CPU()  _NOEXCEPT;
		CPU(CONST CPU&) _NOEXCEPT;
		CPU(CPU&&)      _NOEXCEPT;
		~CPU();

		CPU<T> &operator=(CONST CPU&);
		CPU<T> &operator=(CPU&&);

		VOID push(crVal_, MemoryStorage);
		VOID push(rrVal_, MemoryStorage);
		VOID push(REG, MemoryStorage);
		VOID push(std::streampos);
		VOID pop(MemoryStorage);

		std::streampos top() const _NOEXCEPT;

		VOID add();
		VOID sub();
		VOID mul();
		VOID div();
		VOID dup();

		VOID sqrt();
		VOID sin();
		VOID cos();

		std::pair<T, T> getPair();

		VOID swap(CPU&) _NOEXCEPTARG(std::_Is_nothrow_swappable<T>);

		VOID move(REG, REG);
		VOID move(crVal_, REG);

		VOID dump(std::ostream& = std::cout) const;
		
	private:
		Register<T>           reg_;
		Ram<T>                ram_;
		Stack<T>              stack_;
		Stack<std::streampos> funcRetAddr_;
	};

	template<typename T>
	inline CPU<T>::CPU() _NOEXCEPT :
		reg_(),
		ram_(),
		stack_(),
		funcRetAddr_()
	{ 
		static_assert(std::is_arithmetic<T>::value, "Wrong type in CPU\n");
		LOG_CONSTRUCTING()
	}

	template<typename T>
	inline CPU<T>::CPU(CONST CPU &crCPU) _NOEXCEPT :
		reg_(crCPU.reg_),
		ram_(crCPU.ram_),
		stack_(crCPU.stack_),
		funcRetAddr_(crCPU.funcRetAddr_)
	{
		static_assert(std::is_arithmetic<T>::value, "Wrong type in CPU\n");
		LOG_CONSTRUCTING()
	}

	template<typename T>
	inline CPU<T>::CPU(CPU<T> &&rrCPU) _NOEXCEPT :
		reg_(std::move(rrCPU.reg_)),
		ram_(std::move(rrCPU.ram_)),
		stack_(std::move(rrCPU.stack_)),
		funcRetAddr_(std::move(rrCPU.funcRetAddr_))
	{
		static_assert(std::is_arithmetic<T>::value, "Wrong type in CPU\n");
		LOG_CONSTRUCTING()
	}

	template<typename T>
	inline CPU<T>::~CPU()
	{
		LOG_DESTRUCTING()
	}

	template<typename T>
	inline CPU<T> &CPU<T>::operator=(CONST CPU &crCPU)
	{
		if (this != &crCPU)
		{
			reg_         = crCPU.reg_;
			ram_         = crCPU.ram_;
			stack_       = crCPU.stack_;
			funcRetAddr_ = crCPU.funcRetAddr_;
		}

		return (*this);
	}

	template<typename T>
	inline CPU<T> &CPU<T>::operator=(CPU<T> &&rrCPU)
	{
		assert(this != &rrCPU);

		reg_		 = std::move(rrCPU.reg_);
		ram_		 = std::move(rrCPU.ram_);
		stack_       = std::move(rrCPU.stack_);
		funcRetAddr_ = std::move(rrCPU.funcRetAddr_);

		return (*this);
	}

	template<typename T>
	inline VOID CPU<T>::push(crVal_ val, MemoryStorage memory)
	{ 
		LOG_ARGS(crVal_, val)

		if (memory == MemoryStorage::STACK)
		{
			stack_.push(val);

			reg_[static_cast<SIZE_T>(REG::SP)] = stack_.top();
			reg_.rehash();
		}
		else if (memory == MemoryStorage::RAM) ram_.put(val);
		else NDebugger::Error(std::string("[") + __FUNCTION__ + "] Undefined operation");
	}

	template<typename T>
	inline VOID CPU<T>::push(rrVal_ val, MemoryStorage memory)
	{ 
		LOG_ARGS(rrVal_, val)

		if (memory == MemoryStorage::STACK)
		{
			stack_.push(val);

			reg_[static_cast<SIZE_T>(REG::SP)] = stack_.top();
			reg_.rehash();
		}
		else if (memory == MemoryStorage::RAM) ram_.put(val);		
		else NDebugger::Error(std::string("[") + __FUNCTION__ + "] Undefined operation");
	}

	template<typename T>
	inline VOID CPU<T>::push(REG reg, MemoryStorage memory)
	{	
		LOG_ARGS(SIZE_T, static_cast<SIZE_T>(reg))

		if (memory == MemoryStorage::STACK)
		{
			stack_.push(reg_[static_cast<SIZE_T>(reg)]);

			reg_[static_cast<SIZE_T>(REG::SP)] = stack_.top();
			reg_.rehash();
		}
		else if (memory == MemoryStorage::RAM) ram_.put(reg_[static_cast<SIZE_T>(reg)]);	
		else NDebugger::Error(std::string("[") + __FUNCTION__ + "] Undefined operation");
	}
	
	template<typename T>
	inline VOID CPU<T>::push(std::streampos pos)
	{
		LOG_ARGS(std::streampos, pos)

		funcRetAddr_.push(pos);
	}

	template<typename T>
	inline VOID CPU<T>::pop(MemoryStorage memory)
	{ 
		LOG_FUNC()

		if (memory == MemoryStorage::STACK)
		{
			stack_.pop();

			reg_[static_cast<SIZE_T>(REG::SP)] = stack_.top();
			reg_.rehash();
		}
		else if (memory == MemoryStorage::RAM) ram_.pop();
		else if (memory == MemoryStorage::STACK_FUNC_RET_ADDR) funcRetAddr_.pop();
		else NDebugger::Error(std::string("[") + __FUNCTION__ + "] Undefined operation");
	}

	template<typename T>
	inline std::streampos CPU<T>::top() const _NOEXCEPT
	{	
		LOG_FUNC()

		return funcRetAddr_.top();
	}

	template<typename T>
	VOID CPU<T>::add()
	{
		LOG_FUNC()

		auto a = stack_.top();
		stack_.pop();

		auto b = stack_.top();
		stack_.pop();

		stack_.push(a + b);

		reg_[static_cast<SIZE_T>(REG::SP)] = stack_.top();
		reg_.rehash();
	}

	template<typename T>
	VOID CPU<T>::sub()
	{
		LOG_FUNC()

		auto a = stack_.top();
		stack_.pop();

		auto b = stack_.top();
		stack_.pop();

		stack_.push(a - b);

		reg_[static_cast<SIZE_T>(REG::SP)] = stack_.top();
		reg_.rehash();
	}

	template<typename T>
	VOID CPU<T>::mul()
	{
		LOG_FUNC()
		
		auto a = stack_.top();
		stack_.pop();

		auto b = stack_.top();
		stack_.pop();

		stack_.push(a * b);

		reg_[static_cast<SIZE_T>(REG::SP)] = stack_.top();
		reg_.rehash();
	}

	template<typename T>
	VOID CPU<T>::div()
	{
		LOG_FUNC()

		auto a = stack_.top();
		stack_.pop();

		auto b = stack_.top();
		stack_.pop();

		if (!b) throw std::logic_error("[CPU::div] \"Division by zero\"\n");
		stack_.push(a / b);

		reg_[static_cast<SIZE_T>(REG::SP)] = stack_.top();
		reg_.rehash();
	}

	template<typename T>
	inline VOID CPU<T>::dup()
	{
		LOG_FUNC()

		stack_.push(stack_.top());
	}

	template<typename T>
	VOID CPU<T>::sqrt()
	{
		LOG_FUNC()

#pragma warning(push)
#pragma warning(disable : 4127) // The conditional expression is a constant
		if (std::is_arithmetic<T>()) assert(!"Type T must be arithmetic\n");
#pragma warning(pop)

		auto a = stack_.top();
		stack_.pop();

		stack_.push(static_cast<T>(sqrt_(std::is_integral<T>::value ? static_cast<DOUBLE>(a) : a)));

		reg_[static_cast<SIZE_T>(REG::SP)] = stack_.top();
		reg_.rehash();
	}

	template<typename T>
	VOID CPU<T>::sin()
	{
		LOG_FUNC()

#pragma warning(push)
#pragma warning(disable : 4127) // The conditional expression is a constant
		if (std::is_arithmetic<T>::value) assert(!"Type T must be arithmetic\n");
#pragma warning(pop)

		auto a = stack_.top();
		stack_.pop();

		stack_.push(static_cast<T>(sin_(std::is_integral<T>::value ? static_cast<DOUBLE>(a) : a)));

		reg_[static_cast<SIZE_T>(REG::SP)] = stack_.top();
		reg_.rehash();
	}

	template<typename T>
	VOID CPU<T>::cos()
	{
		LOG_FUNC()

#pragma warning(push)
#pragma warning(disable : 4127) // The conditional expression is a constant
		if (std::is_arithmetic<T>::value) assert(!"Type T must be arithmetic\n");
#pragma warning(pop)

		auto a = stack_.top();
		stack_.pop();

		stack_.push(static_cast<T>(cos_(std::is_integral<T>::value ? static_cast<DOUBLE>(a) : a)));

		reg_[static_cast<SIZE_T>(REG::SP)] = stack_.top();
		reg_.rehash();
	}

	template<typename T>
	std::pair<T, T> CPU<T>::getPair()
	{
		LOG_FUNC()
			
		std::pair<T, T> pair;

		pair.first = stack_.top();
		stack_.pop();

		pair.second = stack_.top();
		stack_.pop();

		reg_[static_cast<SIZE_T>(REG::SP)] = stack_.top();
		reg_.rehash();

		return pair;
	}

	template<typename T>
	inline VOID CPU<T>::swap(CPU &rCPU) _NOEXCEPTARG(std::_Is_nothrow_swappable<T>)
	{ 
		reg_.swap(rCPU.reg_);
		stack_.swap(rCPU.stack_); 
		ram_.swap(rCPU.ram_);
		funcRetAddr_.swap(rCPU.funcRetAddr_);
	}

	template<typename T>
	inline VOID CPU<T>::move(REG src, REG dest) 
	{ 
		LOG_ARGS(SIZE_T, static_cast<SIZE_T>(src), static_cast<SIZE_T>(dest))
		
		reg_[static_cast<SIZE_T>(dest)] = reg_[static_cast<SIZE_T>(src)];
		reg_.rehash();
	}

	template<typename T>
	inline VOID CPU<T>::move(crVal_ src, REG dest) 
	{ 
		LOG_ARGS(SIZE_T, src, reinterpret_cast<crVal_>(dest))

		reg_[static_cast<SIZE_T>(dest)] = src;
		reg_.rehash();
	}

	template<typename T>
	VOID CPU<T>::dump(std::ostream &rOstr /* = std::cout */) const
	{
		LOG_FUNC()
			
		NDebugger::Info("\n\t\t[CPU DUMP]", NDebugger::TextColor::LightMagenta, TRUE, rOstr);

		rOstr << "CPU <" << typeid(T).name() << "> [0x" << this << "]\n\n";

		reg_.dump();
		ram_.dump();
		stack_.dump();

		NDebugger::Info("\t\t[  END   ]\n", NDebugger::TextColor::LightMagenta, TRUE, rOstr);
	}
} // namespace NCpu


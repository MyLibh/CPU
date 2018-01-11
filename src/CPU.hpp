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

	template<typename T = int>
	class CPU final
	{
	public:
		typedef       T  &rVal_;
		typedef       T &&rrVal_;
		typedef const T  &crVal_;

		enum class MemoryStorage
		{
			REGISTER,
			RAM,
			STACK,
			STACK_FUNC_RET_ADDR
		};

		explicit CPU()  noexcept;
		CPU(const CPU&) noexcept;
		CPU(CPU&&)      noexcept;
		~CPU();

		CPU<T> &operator=(const CPU&) noexcept;
		CPU<T> &operator=(CPU&&)      noexcept ;

		void push(crVal_, MemoryStorage);
		void push(rrVal_, MemoryStorage);
		void push(REG, MemoryStorage);
		void push(std::streampos);
		void pop(MemoryStorage);

		std::streampos top() const noexcept;

		void add();
		void sub();
		void mul();
		void div();
		void dup();

		void sqrt();
		void sin();
		void cos();

		std::pair<T, T> getPair();

		void swap(CPU&) noexcept(std::_Is_nothrow_swappable<T>::value);

		void move(REG, REG);
		void move(crVal_, REG);

		void dump(std::ostream& = std::cout) const;
		
	private:
		Register<T>           reg_;
		Ram<T>                ram_;
		Stack<T>              stack_;
		Stack<std::streampos> funcRetAddr_;
	};

	//====================================================================================================================================
	//=========================================================METHOD_DEFINITION==========================================================
	//====================================================================================================================================

#pragma region METHOD_DEFINITION

	template<typename T>
	inline CPU<T>::CPU() noexcept :
		reg_(),
		ram_(),
		stack_(),
		funcRetAddr_()
	{ 
		static_assert(std::is_arithmetic<T>::value, "Wrong type in CPU\n");
		LOG_CONSTRUCTING()
	}

	template<typename T>
	inline CPU<T>::CPU(const CPU &crCPU) noexcept :
		reg_(crCPU.reg_),
		ram_(crCPU.ram_),
		stack_(crCPU.stack_),
		funcRetAddr_(crCPU.funcRetAddr_)
	{
		static_assert(std::is_arithmetic<T>::value, "Wrong type in CPU\n");
		LOG_CONSTRUCTING()
	}

	template<typename T>
	inline CPU<T>::CPU(CPU<T> &&rrCPU) noexcept :
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
	inline CPU<T> &CPU<T>::operator=(const CPU &crCPU) noexcept
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
	inline CPU<T> &CPU<T>::operator=(CPU<T> &&rrCPU) noexcept
	{
		assert(this != &rrCPU);

		reg_		 = std::move(rrCPU.reg_);
		ram_		 = std::move(rrCPU.ram_);
		stack_       = std::move(rrCPU.stack_);
		funcRetAddr_ = std::move(rrCPU.funcRetAddr_);

		return (*this);
	}

	template<typename T>
	inline void CPU<T>::push(crVal_ val, MemoryStorage memory)
	{ 
		LOG_ARGS(crVal_, val)

		if (memory == MemoryStorage::STACK)
		{
			stack_.push(val);

			reg_[static_cast<size_t>(REG::SP)] = stack_.top();
			reg_.rehash();
		}
		else if (memory == MemoryStorage::RAM) ram_.put(val);
		else NDebugger::Error(std::string("[") + __FUNCTION__ + "] Undefined operation");
	}

	template<typename T>
	inline void CPU<T>::push(rrVal_ val, MemoryStorage memory)
	{ 
		LOG_ARGS(rrVal_, val)

		if (memory == MemoryStorage::STACK)
		{
			stack_.push(val);

			reg_[static_cast<size_t>(REG::SP)] = stack_.top();
			reg_.rehash();
		}
		else if (memory == MemoryStorage::RAM) ram_.put(val);		
		else NDebugger::Error(std::string("[") + __FUNCTION__ + "] Undefined operation");
	}

	template<typename T>
	inline void CPU<T>::push(REG reg, MemoryStorage memory)
	{	
		LOG_ARGS(std::string_view, NRegister::GetReg(reg))

		if (memory == MemoryStorage::STACK)
		{
			stack_.push(reg_[static_cast<size_t>(reg)]);

			reg_[static_cast<size_t>(REG::SP)] = stack_.top();
			reg_.rehash();
		}
		else if (memory == MemoryStorage::RAM) ram_.put(reg_[static_cast<size_t>(reg)]);	
		else NDebugger::Error(std::string("[") + __FUNCTION__ + "] Undefined operation");
	}
	
	template<typename T>
	inline void CPU<T>::push(std::streampos pos)
	{
		LOG_ARGS(std::streampos, pos)

		funcRetAddr_.push(pos);
	}

	template<typename T>
	inline void CPU<T>::pop(MemoryStorage memory)
	{ 
		LOG_FUNC()

		if (memory == MemoryStorage::STACK)
		{
			stack_.pop();

			reg_[static_cast<size_t>(REG::SP)] = stack_.top();
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
	void CPU<T>::add()
	{
		LOG_FUNC()

		auto a = stack_.top();
		stack_.pop();

		auto b = stack_.top();
		stack_.pop();

		stack_.push(a + b);

		reg_[static_cast<size_t>(REG::SP)] = stack_.top();
		reg_.rehash();
	}

	template<typename T>
	void CPU<T>::sub()
	{
		LOG_FUNC()

		auto a = stack_.top();
		stack_.pop();

		auto b = stack_.top();
		stack_.pop();

		stack_.push(a - b);

		reg_[static_cast<size_t>(REG::SP)] = stack_.top();
		reg_.rehash();
	}

	template<typename T>
	void CPU<T>::mul()
	{
		LOG_FUNC()
		
		auto a = stack_.top();
		stack_.pop();

		auto b = stack_.top();
		stack_.pop();

		stack_.push(a * b);

		reg_[static_cast<size_t>(REG::SP)] = stack_.top();
		reg_.rehash();
	}

	template<typename T>
	void CPU<T>::div()
	{
		LOG_FUNC()

		auto a = stack_.top();
		stack_.pop();

		auto b = stack_.top();
		stack_.pop();

		if (!b) throw std::logic_error(std::string("[") + __FUNCTION__ + "] Division by zero\n");
		stack_.push(a / b);

		reg_[static_cast<size_t>(REG::SP)] = stack_.top();
		reg_.rehash();
	}

	template<typename T>
	inline void CPU<T>::dup()
	{
		LOG_FUNC()

		stack_.push(stack_.top());
	}

	template<typename T>
	void CPU<T>::sqrt()
	{
		LOG_FUNC()

#pragma warning(push)
#pragma warning(disable : 4127) // The conditional expression is a constant
		if (std::is_arithmetic<T>()) assert(!"Type T must be arithmetic\n");
#pragma warning(pop)

		auto a = stack_.top();
		stack_.pop();

		stack_.push(static_cast<T>(sqrt_(std::is_integral<T>::value ? static_cast<double>(a) : a)));

		reg_[static_cast<size_t>(REG::SP)] = stack_.top();
		reg_.rehash();
	}

	template<typename T>
	void CPU<T>::sin()
	{
		LOG_FUNC()

#pragma warning(push)
#pragma warning(disable : 4127) // The conditional expression is a constant
		if (std::is_arithmetic<T>::value) assert(!"Type T must be arithmetic\n");
#pragma warning(pop)

		auto a = stack_.top();
		stack_.pop();

		stack_.push(static_cast<T>(sin_(std::is_integral<T>::value ? static_cast<double>(a) : a)));

		reg_[static_cast<size_t>(REG::SP)] = stack_.top();
		reg_.rehash();
	}

	template<typename T>
	void CPU<T>::cos()
	{
		LOG_FUNC()

#pragma warning(push)
#pragma warning(disable : 4127) // The conditional expression is a constant
		if (std::is_arithmetic<T>::value) assert(!"Type T must be arithmetic\n");
#pragma warning(pop)

		auto a = stack_.top();
		stack_.pop();

		stack_.push(static_cast<T>(cos_(std::is_integral<T>::value ? static_cast<double>(a) : a)));

		reg_[static_cast<size_t>(REG::SP)] = stack_.top();
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

		reg_[static_cast<size_t>(REG::SP)] = stack_.top();
		reg_.rehash();

		return pair;
	}

	template<typename T>
	inline void CPU<T>::swap(CPU &rCPU) noexcept(std::_Is_nothrow_swappable<T>::value)
	{ 
		reg_.swap(rCPU.reg_);
		stack_.swap(rCPU.stack_); 
		ram_.swap(rCPU.ram_);
		funcRetAddr_.swap(rCPU.funcRetAddr_);
	}

	template<typename T>
	inline void CPU<T>::move(REG src, REG dest) 
	{ 
		LOG_ARGS(size_t, static_cast<size_t>(src), static_cast<size_t>(dest))
		
		reg_[static_cast<size_t>(dest)] = reg_[static_cast<size_t>(src)];
		reg_.rehash();
	}

	template<typename T>
	inline void CPU<T>::move(crVal_ src, REG dest) 
	{ 
		LOG_ARGS(size_t, src, reinterpret_cast<crVal_>(dest))

		reg_[static_cast<size_t>(dest)] = src;
		reg_.rehash();
	}

	template<typename T>
	void CPU<T>::dump(std::ostream &rOstr /* = std::cout */) const
	{
		LOG_FUNC()
			
		NDebugger::Info("\n\t\t[CPU DUMP]", NDebugger::TextColor::LightMagenta, true, rOstr);

		rOstr << "CPU <" << typeid(T).name() << "> [0x" << this << "]\n\n";

		reg_.dump();
		ram_.dump();
		stack_.dump();

		NDebugger::Info("\t\t[  END   ]\n", NDebugger::TextColor::LightMagenta, true, rOstr);
	}

#pragma endregion

} // namespace NCpu


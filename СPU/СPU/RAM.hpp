#pragma once

#include <array>

#include "MyTypedefs.hpp"
#include "Debugger.hpp"

namespace NRam
{
	template<typename T = INT>
	struct RAM final
	{
	private:
		SIZE_T counter_;

	public:
		static CONST SIZE_T RAM_SIZE = 1 << 8;

		std::array<T, RAM_SIZE> buf;

		explicit RAM() noexcept;
		RAM(CONST RAM<T>&) noexcept;
		RAM(RAM<T>&&) noexcept;
		~RAM();

		RAM<T> &operator=(CONST RAM<T>&);
		RAM<T> &operator=(RAM<T>&&);

		inline VOID swap(RAM<T>&) noexcept(std::_Is_nothrow_swappable<T>::value);

		VOID dump() const;
	};

	template<typename T>
	RAM<T>::RAM() noexcept :
		buf()
	{ }

	template<typename T>
	RAM<T>::RAM(CONST RAM<T> &crRAM) noexcept :
		buf(crRAM.buf)
	{ }
		
	template<typename T>
	RAM<T>::RAM(RAM<T> &&rrRAM) noexcept :
		buf(std::move(rrRAM.buf))
	{ }

	template<typename T>
	RAM<T>::~RAM()
	{ }

	template<typename T>
	RAM<T> &RAM<T>::operator=(CONST RAM<T> &crRAM)
	{
		if(this != &crRAM) buf = crRAM.buf;

		return *this;
	}

	template<typename T>
	RAM<T> &RAM<T>::operator=(RAM<T> &&rrRAM)
	{
		assert(this != &rrRAM);

		buf = std::move(rrRAM.buf);

		return *this;
	}

	template<typename T>
	inline VOID swap(RAM<T> &rRAM) noexcept(std::_Is_nothrow_swappable<T>::value)
	{
		buf.swap(rRAM.buf);
	}

	template<typename T>
	VOID RAM<T>::dump() const
	{
		NDebugger::Info("\t[RAM DUMP]", NDebugger::TextColors::LightCyan);
		
		std::cout << "RAM <" << typeid(T).name() << "> [0x" << this << "]\n{\n"
			      << "\tram [" << RAM_SIZE << "] = 0x" << &buf << "\n\t{\n\t\t";

		for (SIZE_T i = 0; i < RAM_SIZE; i += 8)
		{
			for (WORD j = 0; j < 8; ++j) std::cout << "[" << std::setw(3) << i + j << "] = " << buf[i] << (j + i + 1 == RAM_SIZE ? " " : ", ") << "  ";

			std::cout << "\n\t\t";
		}

		std::cout << "\n\t}\n}\n";

		NDebugger::Info("\t[  END   ]\n", NDebugger::TextColors::LightCyan);
	}
} // namespace NRam
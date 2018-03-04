#pragma once

#pragma once

#include <array>     // std::array
#include <iostream>  // std::cout
#include <Windows.h> // SleepEx

#include "..\Register.hpp"

using namespace NRegister;

namespace NRegisterTests
{
	void CopyMoveOperatorsAndConstructorsSwap();
	void DumpOkLog();

	typedef void(*test_func_t)();

	constexpr size_t REGISTER_TEST_FUNC_NUM = 2;

	constexpr std::array<test_func_t, REGISTER_TEST_FUNC_NUM> REGISTER_TEST_FUNC
	{
		CopyMoveOperatorsAndConstructorsSwap,
		DumpOkLog
	};

	void RunAllTests()
	{
		float step     = 100.f / REGISTER_TEST_FUNC_NUM,
			  progress = 0;
		for (auto it = REGISTER_TEST_FUNC.cbegin(); it != REGISTER_TEST_FUNC.cend(); ++it)
		{
			(*it)();

			std::cout << '\r' << "Register tests complete progress: " << (progress += step) << '%';
			SleepEx(500, false);
		}

		std::cout << std::endl;
	}

	void CopyMoveOperatorsAndConstructorsSwap()
	{
		Register<> a;
		Register<> b(a);
		Register<> c(std::move(b));
		Register<> d = a;
		Register<> e = std::move(d);

		c.swap(e);
	}

	void DumpOkLog()
	{
		Register<> a;
		assert(a.ok());

		std::cout << std::endl;
		a.dump(std::clog);

		Logger l;
		l << a;
	}

} // namespace NStackTests
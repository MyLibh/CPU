#pragma once

#include <array> // std::array

#include "..\Stack.hpp"

using namespace NStack;

namespace NStackTests
{
	void CopyMoveOperatorsAndConstructorsSwap();
	void PushPopTopSize();
	void DumpOkLog();

	typedef void(*test_func_t)();

	constexpr size_t STACK_TEST_FUNC_NUM = 3;

	constexpr std::array<test_func_t, STACK_TEST_FUNC_NUM> STACK_TEST_FUNC
	{
		CopyMoveOperatorsAndConstructorsSwap,
		PushPopTopSize,
		DumpOkLog
	};

	void RunAllTests()
	{
		float step     = 100.f / STACK_TEST_FUNC_NUM,
			  progress = 0;
		for (auto it = STACK_TEST_FUNC.cbegin(); it != STACK_TEST_FUNC.cend(); ++it)
		{
			(*it)();

			std::cout << '\r' << "Stack tests complete progress: " << (progress += step) << '%';
			SleepEx(500, false);
		}

		std::cout << std::endl;
	}

	void CopyMoveOperatorsAndConstructorsSwap()
	{
		Stack<> a;
		Stack<> b(a);
		Stack<> c(std::move(b));
		Stack<> d = a;
		Stack<> e = std::move(d);

		c.swap(e);
	}

	void PushPopTopSize()
	{
		Stack<> a;
		a.push(7);
		assert(a.top() == 7);

		a.push(4);
		assert(a.size() == 2);

		a.pop();
		assert(a.top() == 7);
	}

	void DumpOkLog()
	{
		Stack<> a;
		assert(a.ok());

		std::cout << std::endl;
		a.dump(std::clog);

		Logger l;
		l << a;
	}

} // namespace NStackTests
#pragma once

#include <array>     // std::array
#include <iostream>  // std::cout
#include <Windows.h> // SleepEx

#include "..\Storage.hpp"

namespace NStorageTests
{
	void CopyMoveOperatorsAndConstructorsSwap();
	void AtSize();
	void DumpOkOut();

	typedef void(*test_func_t)();

	constexpr size_t STORAGE_TEST_FUNC_NUM = 3;

	constexpr std::array<test_func_t, STORAGE_TEST_FUNC_NUM> STORAGE_TEST_FUNC
	{
		CopyMoveOperatorsAndConstructorsSwap,
		AtSize,
		DumpOkOut
	};

	void RunAllTests()
	{
		float step = 100.f / STORAGE_TEST_FUNC_NUM,
			progress = 0;
		for (auto it = STORAGE_TEST_FUNC.cbegin(); it != STORAGE_TEST_FUNC.cend(); ++it)
		{
			(*it)();

			std::cout << '\r' << "Storage tests complete progress: " << (progress += step) << '%';
			SleepEx(500, false);
		}

		std::cout << std::endl;
	}

	void CopyMoveOperatorsAndConstructorsSwap()
	{
		Storage<int, 5> a;
		Storage<int, 5> b(a);
		Storage<int, 5> c(std::move(b));
		Storage<int, 5> d = a;
		Storage<int, 5> e = std::move(d);

		c.swap(e);
	}

	void AtSize()
	{
		Storage<int, 5> a;
		a[0] = 4;
		assert(a[0] == 4);

		assert(a.size() == 5);
	}

	void DumpOkOut()
	{
		Storage<int, 5> a;
		assert(a.ok());

		std::clog << std::endl << a;
	}

} // namespace NStackTests
#pragma once

#include "StackTests.hpp"
#include "StorageTests.hpp"
#include "RegisterTests.hpp"

void RunTestsAutomatic()
{
	NStackTests::RunAllTests();
	NStorageTests::RunAllTests();
	NRegisterTests::RunAllTests();
}


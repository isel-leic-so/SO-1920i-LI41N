#include "stdafx.h"
#include <Windows.h>
#include "../Include/UThread.h"

#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace Uthreads2UnitTests
{	
	TEST_MODULE_INITIALIZE(StartTests)
	{
		UtInit();
	}

	TEST_MODULE_CLEANUP(EndTests)
	{
		UtEnd();
	}


	TEST_CLASS(UthreadTests)
	{
	private:

		//Thread function
		static LONG Func1(UT_ARGUMENT arg) {
			LONG r = (LONG)arg;
			return r;
		}

	public:
		

		TEST_METHOD(SimpleGetThreadResultTest)
		{
			UtInit();
			LONG expected = 99L;
			LONG result;

			HANDLE t1 = UtCreate(Func1, (UT_ARGUMENT) expected);

			UtRun();
			Assert::IsTrue(UtGetThreadResult(t1, &result));
			Assert::AreEqual(expected, result);

			UtEnd();
		}

	};
}
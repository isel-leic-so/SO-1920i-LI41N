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

	// auxiliary type to BlockingGetThreadResultTest test
	typedef struct _RESULT_PARMS {
		HANDLE hThread;
		LONG result;
		INT waitTime;
	} RESULT_PARMS, *PRESULT_PARMS;


	TEST_CLASS(UthreadTests)
	{
	private:
		static const  int  WAIT_TIME = 5000;


		//Thread function
		static LONG Func1(UT_ARGUMENT arg) {
			LONG r = (LONG)arg;
			return r;
		}

		// Thread to obtain result
		static LONG Func2(UT_ARGUMENT arg) {
			LONG r = (LONG)arg;
			Sleep(WAIT_TIME);
			return r;
		}

		// Thread getting the result
		static LONG Func3(UT_ARGUMENT arg) {
			PRESULT_PARMS parms = (PRESULT_PARMS)arg;
			LONG r;

			LONG startTime = GetTickCount();
			UtGetThreadResult(parms->hThread, &r);
			LONG endTime = GetTickCount();

			parms->result = r;
			parms->waitTime = endTime - startTime;
			return r;
		}


	public:
		

		TEST_METHOD(SimpleGetThreadResultTest)
		{
			
			LONG expected = 99L;
			LONG result;

			HANDLE t1 = UtCreate(Func1, (UT_ARGUMENT) expected);

			UtRun();
			Assert::IsTrue(UtTryGetThreadResult(t1, &result));
			Assert::AreEqual(expected, result);

		}

		TEST_METHOD(BlockingGetThreadResultTest)
		{
			RESULT_PARMS parms;
			LONG expected = 99L;
		 
			
			// The result consumer thread
			HANDLE t3 = UtCreate(Func3, (UT_ARGUMENT)&parms);

			// The result provider thread
			parms.hThread = UtCreate(Func2, (UT_ARGUMENT)expected);

		

			UtRun();
			Assert::IsTrue(parms.waitTime >= WAIT_TIME);
			Assert::AreEqual(expected, parms.result);

		}

	};
}
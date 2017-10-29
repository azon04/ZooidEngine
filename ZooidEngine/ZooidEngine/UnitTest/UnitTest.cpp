#include "UnitTest.h"

#include "ZEngine.h"

#include "VectorTest.h"
#include "ArrayTest.h"
#include "HashMapTest.h"

void UnitTest::RunMainTest()
{
	VectorUnitTest vectorTest;
	vectorTest.RunTest();
	ZEINFO("Vector Test Completed");

	ArrayTest arrayTest;
	arrayTest.RunTest();
	ZEINFO("Vector Test Completed");

	HashMapTest hashMapTest;
	hashMapTest.RunTest();
	ZEINFO("HashMap Test Completed");

}

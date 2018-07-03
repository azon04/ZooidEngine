#include "UnitTest.h"

#include "Utils/ZEngineHelper.h"

#include "VectorTest.h"
#include "ArrayTest.h"
#include "HashMapTest.h"
#include "FileTest.h"

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

	FileTest fileTest;
	fileTest.RunTest();
	ZEINFO("File Test Completed");

}

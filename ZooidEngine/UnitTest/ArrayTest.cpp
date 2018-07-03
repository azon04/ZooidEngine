#include "ArrayTest.h"

#include "Utils/ZEngineHelper.h"

#include "Utils/Array.h"

void ArrayTest::RunTest()
{
	ZE::Array<int, false> staticArray(4);

	staticArray.push_back(3);
	staticArray.push_back(3);
	staticArray.push_back(3);
	staticArray.push_back(3);
	ZASSERT(staticArray[3] == 3, "Static Array Assign Failed");
	ZASSERT(staticArray.length() == 4, "Check length for Static Array Failed");
	ZASSERT(staticArray.capacity() == 4, "Check Capacity for Static Array Failed");

	ZE::Array<int, true> dynamicArray;

	dynamicArray.push_back(4);
	ZASSERT(dynamicArray[0] == 4, "Put Function for Dynamic Array Failed");

	dynamicArray.push_back(5);
	ZASSERT(dynamicArray.capacity() == 2, "Resize for Dynamic Array Failed");
	ZASSERT(dynamicArray.length() == 2, "Check length for Dynamic Array Failed");

	int popValue = dynamicArray.pop();
	ZASSERT(popValue == 5, "Pop fir Dynamic Array Failed");

	dynamicArray.push_back(7);
	dynamicArray.removeAt(0);
	ZASSERT(dynamicArray.length() == 1, "RemoveAt for Dynamic Array Failed");
	ZASSERT(dynamicArray[0] == 7, "Shifting in RemoveAt for Dynamic Array Failed");

}


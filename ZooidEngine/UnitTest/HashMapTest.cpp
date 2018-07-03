#include "HashMapTest.h"

#include "Utils/ZEngineHelper.h"

#include "Utils/HashMap.h"

void HashMapTest::RunTest()
{
	ZE::HashMap<ZE::String, int> testMap;
	testMap.put("Hai", 3);
	ZASSERT(testMap["Hai"] == 3, "Put HashMap Failed");

	testMap.put("Hau", 4);
	testMap.put("Hau", 5);
	ZASSERT(testMap["Hau"] == 5, "Put HashMap Failed");

	testMap.put("Hal", 12);
	ZASSERT(testMap["Hal"] == 12, "Put HashMap Failed");

	testMap.put("Hai", 1);
	ZASSERT(testMap["Hai"] == 1, "Put HashMap Failed");

	testMap.put("Biji1", 3);
	testMap.put("Biji2", 7);
	testMap.put("Biji3", 9);
	testMap.put("Biji1", 1234);
	testMap.put("Biji3", 567);
	testMap.put("Biji4", 4321);
	ZASSERT(testMap["Biji4"] == 4321, "Put HashMap Failed");

	testMap.erase("Hai");
	ZASSERT(!testMap.hasKey("Hai"), "Erase HashMap Failed");

	ZE::HashMap<ZE::Int32, ZE::Int32> intTestMap;
	intTestMap.put(3, 3);
	ZASSERT(intTestMap[3] == 3, "Put HashMap Failed");

	intTestMap.put(4, 4);
	ZASSERT(intTestMap[4] == 4, "Put HashMap Failed");

	intTestMap.put(4, 5);
	ZASSERT(intTestMap[4] == 5, "Put HashMap Failed");

	intTestMap.erase(3);
	ZASSERT(!intTestMap.hasKey(3), "Erase HashMap Failed");
}


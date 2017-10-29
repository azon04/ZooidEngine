#include "HashMapTest.h"
#include "ZEngine.h"

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
}


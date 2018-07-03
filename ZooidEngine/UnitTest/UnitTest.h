#ifndef __ZENGINE_UNIT_TEST__
#define __ZENGINE_UNIT_TEST__

class UnitTest 
{
	
public:
	virtual void RunTest() = 0;

	static void RunMainTest();
};

#endif

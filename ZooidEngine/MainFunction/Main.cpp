#include "ZEGameContext.h"
#include "MainFunctions.h"
#include "Application/Application.h"
#include "UnitTest/UnitTest.h"
#include "Renderer/DrawList.h"
#include "Platform/Platform.h"
#include "Profiler/ProfilerZooid.h"

ZE_MAIN()
{
	ZE_INITPLATFORM();

	// Init Engine
	ZE::GameContext gameContext;

	ZE::MainSetup(&gameContext);
	
	ZE_POSTINITPLATFORM();

	ZE_PROFILER_INIT();
	//UnitTest::RunMainTest();

	// Main Loop
	ZE_PLATFORMLOOP()
	{
		ZE_PROFILER_FRAME("Main");
		ZE::MainThreadJob(&gameContext);
	}

	for(int i=0; i < MAX_DRAWLIST_BUFFER; i++)
	{ 
		gameContext.getDrawList(i)->Reset();
	}

	ZE::MainClean(&gameContext);

	ZE_PROFILER_DESTROY();

	ZE_DESTROYPLATFORM();

}
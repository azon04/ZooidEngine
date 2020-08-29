#include "ZEGameContext.h"
#include "MainFunctions.h"
#include "Application/Application.h"
#include "UnitTest/UnitTest.h"
#include "Renderer/DrawList.h"
#include "Platform/Platform.h"

ZE_MAIN()
{
	ZE_INITPLATFORM();

	// Init Engine
	ZE::GameContext gameContext;

	ZE::MainSetup(&gameContext);
	
	ZE_POSTINITPLATFORM();

	//UnitTest::RunMainTest();

	// Main Loop
	ZE_PLATFORMLOOP()
	{
		ZE::MainThreadJob(&gameContext);
	}

	for(int i=0; i < MAX_DRAWLIST_BUFFER; i++)
	{ 
		gameContext.getDrawList(i)->Reset();
	}

	ZE::MainClean(&gameContext);

	ZE_DESTROYPLATFORM();

}
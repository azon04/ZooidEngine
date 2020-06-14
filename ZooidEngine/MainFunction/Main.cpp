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
	
	//UnitTest::RunMainTest();

	// Main Loop
	ZE_PLATFORMLOOP()
	{
		ZE::MainThreadJob(&gameContext);
	}

	gameContext.getDrawList()->Reset();

	ZE::MainClean(&gameContext);

	ZE_DESTROYPLATFORM();

}
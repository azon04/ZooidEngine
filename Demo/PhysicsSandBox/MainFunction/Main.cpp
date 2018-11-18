#include "ZooidEngine/ZEGameContext.h"
#include "ZooidEngine/MainFunction/MainFunctions.h"
#include "ZooidEngine/UnitTest/UnitTest.h"
#include "ZooidEngine/Renderer/DrawList.h"
#include "ZooidEngine/Platform/Platform.h"

#include "MainFunctions.h"

ZE_MAIN()
{
	ZE_INITPLATFORM();

	// Init Engine
	ZE::GameContext gameContext;

	ZE::MainSetup(&gameContext);

	PhysicsSandBox::MainSetup(&gameContext);

	// Main Loop
	ZE_PLATFORMLOOP()
	{
		ZE::MainThreadJob(&gameContext);
	}

	gameContext.getDrawList()->Reset();

	PhysicsSandBox::MainClean(&gameContext);

	ZE::MainClean(&gameContext);

	ZE_DESTROYPLATFORM();
}
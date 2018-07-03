#include "ZooidEngine/ZEGameContext.h"
#include "ZooidEngine/MainFunction/MainFunctions.h"
#include "ZooidEngine/UnitTest/UnitTest.h"
#include "ZooidEngine/Renderer/DrawList.h"

#include "MainFunction.h"

int main(int argc, char** argv) 
{

	// Init Engine
	ZE::GameContext gameContext;

	ZE::MainSetup(&gameContext);

	AnimationDemo::MainSetup(&gameContext);

	// Main Loop
	while (!gameContext.getRenderer()->IsClose())
	{
		ZE::MainThreadJob(&gameContext);
	}

	gameContext.getDrawList()->Reset();

	AnimationDemo::MainClean(&gameContext);
	ZE::MainClean(&gameContext);

	return 0;
}

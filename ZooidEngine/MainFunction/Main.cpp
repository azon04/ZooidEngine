#include "ZEGameContext.h"
#include "MainFunctions.h"
#include "UnitTest/UnitTest.h"
#include "Renderer/DrawList.h"

int main(int argc, char** argv) 
{
	// Init Engine
	ZE::GameContext gameContext;
	
	ZE::MainSetup(&gameContext);
	
	UnitTest::RunMainTest();

	// Main Loop
	while (!gameContext.getRenderer()->IsClose()) 
	{
		ZE::MainThreadJob(&gameContext);
	}

	gameContext.getDrawList()->Reset();

	ZE::MainClean(&gameContext);

	return 0;

}
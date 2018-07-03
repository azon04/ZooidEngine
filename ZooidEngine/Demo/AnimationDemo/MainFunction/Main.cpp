#include "ZooidEngine/ZEGameContext.h"
#include "ZooidEngine/MainFunction/MainFunctions.h"
#include "ZooidEngine/UnitTest/UnitTest.h"
#include "ZooidEngine/Renderer/DrawList.h"

#include "MainFunction.h"

// TODO for NVIDIA Optimus :  This enable the program to use NVIDIA instead of integrated Intel graphics
#if WIN32 || WIN64
extern "C" {
#include <windows.h>
	_declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
}
#endif

int main(int argc, char** argv) {

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

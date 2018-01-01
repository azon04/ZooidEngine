#include "ZEngine.h"
#include "ZEGameContext.h"

#include "MainFunctions.h"

#include "UnitTest/UnitTest.h"

#if Z_RENDER_OPENGL
#define GLUE_STATIC
#include <GL/glew.h>
#endif

// GLFW
#include <GLFW/glfw3.h>

#include <stdio.h>

int main(int argc, char** argv) {

	// Init Engine
	ZE::GameContext gameContext;
	
	ZE::MainSetup(&gameContext);
	
	UnitTest::RunMainTest();

	// Main Loop
	while (!gameContext.getRenderer()->IsClose()) {
		ZE::MainThreadJob(&gameContext);
	}

	gameContext.getDrawList()->Reset();

	ZE::MainClean(&gameContext);

	return 0;

}
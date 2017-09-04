#include "../UnitTest/VectorTest.h"

#include "../ZEngine.h"
#include "../ZEGameContext.h"

#include "MainFunctions.h"

#define GLUE_STATIC
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>

#include <stdio.h>

int main(int argc, char** argv) {

	// Init Engine
	ZE::GameContext gameContext;
	
	ZE::MainSetup(&gameContext);
	
	ZE::ShaderAction& shaderAction = gameContext.getDrawList()->getNextShaderAction();
	shaderAction.SetShaderAndBuffer(ZE::ShaderManager::getInstance()->m_shaders[0], ZE::BufferManager::getInstance()->m_GPUBufferArrays[0]);
	shaderAction.m_vertexSize = 6;

	// Main Loop
	while (!gameContext.getRenderer()->IsClose()) {
		gameContext.getRenderer()->BeginRender();

		gameContext.getRenderer()->ClearScreen();

		for (int i = 0; i < gameContext.getDrawList()->m_size; i++) {
			gameContext.getRenderer()->Draw(&gameContext.getDrawList()->m_drawList[i]);
		}

		gameContext.getRenderer()->EndRender();
	}

	ZE::MainClean(&gameContext);

	return 0;

}
#include "../UnitTest/VectorTest.h"

#include "../ZEngine.h"
#include "../ZEGameContext.h"

#include "MainFunctions.h"

#if Z_RENDER_OPENGL
#define GLUE_STATIC
#include <GL/glew.h>
#endif

// GLFW
#include <GLFW/glfw3.h>

#include <stdio.h>

#include "../Scene/CameraComponent.h"

int main(int argc, char** argv) {

	// Init Engine
	ZE::GameContext gameContext;
	
	ZE::MainSetup(&gameContext);
	
	{
		ZE::ShaderAction& shaderAction = gameContext.getDrawList()->getNextShaderAction();
		shaderAction.SetType(SHADER_ACTION_SETGLOBAL);

		if (gameContext.getCameraManager()->getCurrentCamera())
		{
			Matrix4x4 mat;
			gameContext.getCameraManager()->m_currentCamera->m_worldTransform.setPos(Vector3(0.5f, 0.5f, -0.5f));
			gameContext.getCameraManager()->m_currentCamera->getViewMatrix(mat);
			shaderAction.SetShaderMatVar("viewMat", mat);
		}
	}

	{
		ZE::ShaderAction& shaderAction = gameContext.getDrawList()->getNextShaderAction();
		shaderAction.SetShaderAndBuffer(ZE::ShaderManager::getInstance()->m_shaders[0], ZE::BufferManager::getInstance()->m_GPUBufferArrays[0]);
		shaderAction.m_vertexSize = 6;
	}
	
	{
		Matrix4x4 mat;
		mat.scale(Vector3(0.5f, 0.5f, 0.5f));
		mat.rotateAroundN(DegToRad(45.0f));

		ZE::ShaderAction& shaderAction = gameContext.getDrawList()->getNextShaderAction();
		ZE::Shader* shader = ZE::ShaderManager::getInstance()->m_shaders[1];
		
		shaderAction.SetShaderAndBuffer(shader, ZE::BufferManager::getInstance()->m_GPUBufferArrays[1]);
		shaderAction.m_vertexSize = 288;
		shaderAction.SetShaderMatVar("modelMat", mat);
	}

	// Main Loop
	while (!gameContext.getRenderer()->IsClose()) {
		gameContext.getRenderer()->BeginRender();

		gameContext.getRenderer()->ClearScreen();

		for (int i = 0; i < gameContext.getDrawList()->m_size; i++) {
			gameContext.getRenderer()->ProcessShaderAction(&gameContext.getDrawList()->m_drawList[i]);
		}

		gameContext.getRenderer()->EndRender();
	}

	ZE::MainClean(&gameContext);

	return 0;

}
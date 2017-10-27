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
#include "../Utils/String.h"
#include "../Utils/HashMap.h"

int main(int argc, char** argv) {

	// Init Engine
	ZE::GameContext gameContext;
	
	ZE::MainSetup(&gameContext);
	
	Matrix4x4 viewMat;
	Matrix4x4 projectionMat;
	Matrix4x4 modelMat;

	{
		ZE::ShaderAction& shaderAction = gameContext.getDrawList()->getNextShaderAction();
		shaderAction.SetType(SHADER_ACTION_SETGLOBAL);

		if (gameContext.getCameraManager()->getCurrentCamera())
		{
			gameContext.getCameraManager()->m_currentCamera->m_worldTransform.setPos(Vector3(0.0f, 0.0f, 5.0f));
			gameContext.getCameraManager()->m_currentCamera->getViewMatrix(viewMat);
			shaderAction.SetShaderMatVar("viewMat", viewMat);
		}
	}

	{
		ZE::ShaderAction& shaderAction = gameContext.getDrawList()->getNextShaderAction();
		shaderAction.SetType(SHADER_ACTION_SETGLOBAL);

		ZE::CameraComponent* currentCamera = gameContext.getCameraManager()->getCurrentCamera();
		if (currentCamera)
		{
			ZE::IRenderer* renderer = gameContext.getRenderer();
			//ZE::MathOps::CreatePerspectiveProj(projectionMat, renderer->GetWidth(), renderer->GetHeight(), currentCamera->m_near, currentCamera->m_far);
			ZE::MathOps::CreatePerspectiveProjEx(projectionMat, renderer->GetWidth() / renderer->GetHeight(), 45.0f, currentCamera->m_near, currentCamera->m_far);
			//ZE::MathOps::CreateOrthoProj(projectionMat, 1.0f * renderer->GetWidth() / renderer->GetHeight(), 1.0f, currentCamera->m_near, currentCamera->m_far);
			shaderAction.SetShaderMatVar("projectionMat", projectionMat);
		}
	}

	{
		//modelMat.translate(Vector3(5.0f, 2.5f, -25.0f));
		modelMat.rotateAroundU(DegToRad(45.0f));
		modelMat.rotateAroundV(DegToRad(45.0f));

		ZE::ShaderAction& shaderAction = gameContext.getDrawList()->getNextShaderAction();
		ZE::Shader* shader = ZE::ShaderManager::getInstance()->m_shaders[1];
		
		shaderAction.SetShaderAndBuffer(shader, ZE::BufferManager::getInstance()->m_GPUBufferArrays[1]);
		shaderAction.m_vertexSize = 288;
		shaderAction.SetShaderMatVar("modelMat", modelMat);
	}

	ZE::HashMap<ZE::String, int> testMap;
	testMap.put("Hai", 3);
	testMap.put("Hau", 4);
	testMap.put("Hau", 5);
	testMap.put("Hal", 12);
	testMap.put("Hai", 1);
	testMap.put("Biji1", 3);
	testMap.put("Biji2", 7);
	testMap.put("Biji3", 9);
	testMap.put("Biji1", 1234);
	testMap.put("Biji3", 567);
	testMap.put("Biji4", 4321);
	
	ZEINFO("Hai value : %d", testMap["Hai"]);
	ZEINFO("Hau value : %d", testMap["Hau"]);
	ZEINFO("Hal value : %d", testMap["Hal"]);
	ZEINFO("Biji1 value : %d", testMap["Biji1"]);
	ZEINFO("Biji2 value : %d", testMap["Biji2"]);
	ZEINFO("Biji3 value : %d", testMap["Biji3"]);
	ZEINFO("Biji4 value : %d", testMap["Biji4"]);

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
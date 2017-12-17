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

#include "Renderer/GPUTexture.h"
#include "Scene/CameraComponent.h"
#include "MemoryManagement/Handle.h"
#include "Events/Events.h"

int main(int argc, char** argv) {

	// Init Engine
	ZE::GameContext gameContext;
	
	ZE::MainSetup(&gameContext);
	
	UnitTest::RunMainTest();

	Matrix4x4 viewMat;
	Matrix4x4 projectionMat;
	Matrix4x4 modelMat;

	{
		ZE::ShaderAction& shaderAction = gameContext.getDrawList()->getNextShaderAction();
		shaderAction.SetType(SHADER_ACTION_SETGLOBAL);

		if (gameContext.getCameraManager()->getCurrentCamera())
		{
			gameContext.getCameraManager()->m_currentCamera->m_worldTransform.setPos(Vector3(1.0f, 1.0f, 5.0f));
			gameContext.getCameraManager()->m_currentCamera->m_worldTransform.rotateAroundV(DegToRad(15));
			gameContext.getCameraManager()->m_currentCamera->m_worldTransform.rotateAroundU(DegToRad(-10));
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
		modelMat.translate(Vector3(-1.f, 0.0f, 0.0f));
		//modelMat.rotateAroundU(DegToRad(45.0f));
		//modelMat.rotateAroundV(DegToRad(45.0f));

		ZE::ShaderAction& shaderAction = gameContext.getDrawList()->getNextShaderAction();
		ZE::ShaderChain* shader = ZE::ShaderManager::getInstance()->getShaderChain(1);
		
		shaderAction.SetShaderAndBuffer(shader, ZE::BufferManager::getInstance()->m_GPUBufferArrays[1]);
		shaderAction.m_vertexSize = 288;
		shaderAction.SetShaderMatVar("modelMat", modelMat);
		ZE::GPUTexture* pGPUTexture = gameContext.getTextureManager()->getResource<ZE::GPUTexture>("../Resources/Textures/container2.png");
		shaderAction.SetShaderTextureVar("material.diffuseMap", pGPUTexture, 0);
	}

	{
		ZE::ShaderAction& shaderAction = gameContext.getDrawList()->getNextShaderAction();
		ZE::ShaderChain* shader = ZE::ShaderManager::getInstance()->getShaderChain(2);

		shaderAction.SetShaderAndBuffer(shader, ZE::BufferManager::getInstance()->m_GPUBufferArrays[2]);
		shaderAction.m_vertexSize = 36;
		shaderAction.SetShaderMatVar("modelMat", Matrix4x4());
	}


	// Main Loop
	while (!gameContext.getRenderer()->IsClose()) {

		// Handle Event_Update
		{
			ZE::Handle handleUpdate("EventUpdate", sizeof(ZE::Event_UPDATE));
			ZE::Event_UPDATE* eventUpdate = new(handleUpdate) ZE::Event_UPDATE();
			gameContext.getEventDispatcher()->handleEvent(eventUpdate);
			gameContext.getInputManager()->handleEvent(eventUpdate);
			handleUpdate.release();
		}

		for (int i = 0; i < gameContext.getEventDispatcher()->getEvents(ZE::EVENT_INPUT).length(); i++)
		{
			ZE::Handle handle = gameContext.getEventDispatcher()->getEvents(ZE::EVENT_INPUT)[i];
			if (handle.isValid())
			{
				gameContext.getEventDispatcher()->handleEvent(handle.getObject<ZE::Event>());
			}
			handle.release();
		}
		gameContext.getEventDispatcher()->clearEvents(ZE::EVENT_INPUT);

		gameContext.getRenderer()->BeginRender();

		gameContext.getRenderer()->ClearScreen();

		for (int i = 0; i < gameContext.getDrawList()->m_size; i++) {
			gameContext.getRenderer()->ProcessShaderAction(&gameContext.getDrawList()->m_drawList[i]);
		}

		gameContext.getRenderer()->EndRender();
	}

	gameContext.getDrawList()->Reset();

	ZE::MainClean(&gameContext);

	return 0;

}
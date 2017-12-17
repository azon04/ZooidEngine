#include "MainFunctions.h"

#include "../Common/GlobalRegistry.h"

#include "Scene/CameraComponent.h"
#include "Renderer/GPUTexture.h"
#include "MemoryManagement/Handle.h"
#include "Events/Events.h"

namespace ZE {

	void MainSetup(GameContext* _gameContext)
	{
		// Construct Memory
		MemoryManager::Construct();

		// Register all classes
		GlobalRegistry::Register();

		{
			Handle handle("RENDERER", sizeof(ZE::GLRenderer));
			_gameContext->m_renderer = new(handle) ZE::GLRenderer();
			_gameContext->m_renderer->Setup();
		}

		ShaderManager::Init();
		_gameContext->m_shaderManager = ShaderManager::getInstance();

		BufferManager::Init();
		_gameContext->m_bufferManager = BufferManager::getInstance();
		
		TextureManager::Init();
		_gameContext->m_textureManager = TextureManager::getInstance();

		{
			Handle handle("DrawList", sizeof(DrawList));
			_gameContext->m_drawList = new(handle) DrawList;
		}

		// Create Main Event Dispatcher
		{
			Handle handle("EventDispatcher", sizeof(EventDispatcher));
			_gameContext->m_mainEventDispatcher = new(handle) EventDispatcher(_gameContext);
		}

		// Create Root Component and add is as child of Event Dispatcher
		{
			Handle handle("RootComponent", sizeof(SceneComponent));
			_gameContext->m_rootComponent = new (handle) SceneComponent(_gameContext);
			_gameContext->getEventDispatcher()->addChild(_gameContext->m_rootComponent);
			_gameContext->m_rootComponent->setupComponent();
		}

		// Create Input Manager
		{
			Handle handle("InputManager", sizeof(InputManager));
			_gameContext->m_inputManager = new (handle) InputManager(_gameContext);
			_gameContext->m_inputManager->setupComponent();
		}

		CameraManager::Init(_gameContext);
		_gameContext->m_cameraManager = CameraManager::GetInstance();
	}

	void MainClean(GameContext* _gameContext)
	{
		BufferManager::Destroy();
		ShaderManager::Destroy();
		TextureManager::Destroy();

		_gameContext->m_renderer->Clean();

		MemoryManager::Deconstruct();
	}

	void MainThreadJob(GameContext* _gameContext)
	{
		Matrix4x4 viewMat;
		Matrix4x4 projectionMat;
		Matrix4x4 modelMat;

		{
			ZE::ShaderAction& shaderAction = _gameContext->getDrawList()->getNextShaderAction();
			shaderAction.SetType(SHADER_ACTION_SETGLOBAL);

			if (_gameContext->getCameraManager()->getCurrentCamera())
			{
				_gameContext->getCameraManager()->m_currentCamera->getViewMatrix(viewMat);
				shaderAction.SetShaderMatVar("viewMat", viewMat);
			}
		}

		{
			ZE::ShaderAction& shaderAction = _gameContext->getDrawList()->getNextShaderAction();
			shaderAction.SetType(SHADER_ACTION_SETGLOBAL);

			ZE::CameraComponent* currentCamera = _gameContext->getCameraManager()->getCurrentCamera();
			if (currentCamera)
			{
				ZE::IRenderer* renderer = _gameContext->getRenderer();
				//ZE::MathOps::CreatePerspectiveProj(projectionMat, renderer->GetWidth(), renderer->GetHeight(), currentCamera->m_near, currentCamera->m_far);
				ZE::MathOps::CreatePerspectiveProjEx(projectionMat, renderer->GetWidth() / renderer->GetHeight(), 45.0f, currentCamera->m_near, currentCamera->m_far);
				//ZE::MathOps::CreateOrthoProj(projectionMat, 1.0f * renderer->GetWidth() / renderer->GetHeight(), 1.0f, currentCamera->m_near, currentCamera->m_far);
				shaderAction.SetShaderMatVar("projectionMat", projectionMat);
			}
		}

		{
			modelMat.translate(Vector3(-1.f, 0.0f, 0.0f));

			ZE::ShaderAction& shaderAction = _gameContext->getDrawList()->getNextShaderAction();
			ZE::ShaderChain* shader = ZE::ShaderManager::getInstance()->getShaderChain(1);

			shaderAction.SetShaderAndBuffer(shader, ZE::BufferManager::getInstance()->m_GPUBufferArrays[1]);
			shaderAction.m_vertexSize = 288;
			shaderAction.SetShaderMatVar("modelMat", modelMat);
			ZE::GPUTexture* pGPUTexture = _gameContext->getTextureManager()->getResource<ZE::GPUTexture>("../Resources/Textures/container2.png");
			shaderAction.SetShaderTextureVar("material.diffuseMap", pGPUTexture, 0);
		}

		{
			ZE::ShaderAction& shaderAction = _gameContext->getDrawList()->getNextShaderAction();
			ZE::ShaderChain* shader = ZE::ShaderManager::getInstance()->getShaderChain(2);

			shaderAction.SetShaderAndBuffer(shader, ZE::BufferManager::getInstance()->m_GPUBufferArrays[2]);
			shaderAction.m_vertexSize = 36;
			shaderAction.SetShaderMatVar("modelMat", Matrix4x4());
		}

		// Handle Event_Update
		{
			ZE::Handle handleUpdate("EventUpdate", sizeof(ZE::Event_UPDATE));
			ZE::Event_UPDATE* eventUpdate = new(handleUpdate) ZE::Event_UPDATE();
			_gameContext->getEventDispatcher()->handleEvent(eventUpdate);
			_gameContext->getInputManager()->handleEvent(eventUpdate);
			handleUpdate.release();
		}

		for (int i = 0; i < _gameContext->getEventDispatcher()->getEvents(ZE::EVENT_INPUT).length(); i++)
		{
			ZE::Handle handle = _gameContext->getEventDispatcher()->getEvents(ZE::EVENT_INPUT)[i];
			if (handle.isValid())
			{
				_gameContext->getEventDispatcher()->handleEvent(handle.getObject<ZE::Event>());
			}
			handle.release();
		}
		_gameContext->getEventDispatcher()->clearEvents(ZE::EVENT_INPUT);

		_gameContext->getRenderer()->BeginRender();

		_gameContext->getRenderer()->ClearScreen();

		for (int i = 0; i < _gameContext->getDrawList()->m_size; i++) {
			_gameContext->getRenderer()->ProcessShaderAction(&_gameContext->getDrawList()->m_drawList[i]);
		}

		_gameContext->getRenderer()->EndRender();

		_gameContext->getDrawList()->Reset();
	}

}
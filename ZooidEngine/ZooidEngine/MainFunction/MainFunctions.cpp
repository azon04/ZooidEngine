#include "MainFunctions.h"

#include "../Common/GlobalRegistry.h"

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

}
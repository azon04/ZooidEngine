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
		
		{
			Handle handle("DrawList", sizeof(DrawList));
			_gameContext->m_drawList = new(handle) DrawList;
		}
	}

	void MainClean(GameContext* _gameContext)
	{

		BufferManager::Destroy();
		ShaderManager::Destroy();
		_gameContext->m_renderer->Clean();

		MemoryManager::Deconstruct();
	}

}
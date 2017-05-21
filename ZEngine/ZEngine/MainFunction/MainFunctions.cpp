#include "MainFunctions.h"

#include "../Common/GlobalRegistry.h"

namespace ZE {

	void MainSetup(GameContext* _gameContext)
	{

		// Register all classes
		GlobalRegistry::Register();

		_gameContext->m_renderer = new ZE::GLRenderer();
		_gameContext->m_renderer->Setup();

		ShaderManager::Init();
		_gameContext->m_shaderManager = ShaderManager::getInstance();

		BufferManager::Init();
		_gameContext->m_bufferManager = BufferManager::getInstance();
		
		_gameContext->m_drawList = new DrawList;
	}

	void MainClean(GameContext* _gameContext)
	{
		delete _gameContext->m_drawList;

		BufferManager::Destroy();
		ShaderManager::Destroy();
		_gameContext->m_renderer->Clean();
	}

}
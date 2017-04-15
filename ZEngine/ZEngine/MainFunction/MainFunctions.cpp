#include "MainFunctions.h"

namespace ZE {

	void MainSetup(GameContext* _gameContext)
	{
		_gameContext->m_renderer = new ZE::GLRenderer();
		_gameContext->m_renderer->Setup();

		BufferManager::Init();
		_gameContext->m_bufferManager = BufferManager::getInstance();
	}

	void MainClean(GameContext* _gameContext)
	{
		BufferManager::Destroy();
		_gameContext->m_renderer->Clean();
	}

}
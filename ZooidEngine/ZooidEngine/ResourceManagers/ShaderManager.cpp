#include "ShaderManager.h"

#include "ZEGameContext.h"
#include "Renderer/RenderZooid.h"

namespace ZE
{
	IMPLEMENT_CLASS_1(ShaderManager, ResourceManager)

	UInt8 ShaderManager::getShaderTypeByName(const char* resourceFilePath)
	{
		Int32 length = StringFunc::Length(resourceFilePath);

		if (resourceFilePath[length - 2] == 'v' && resourceFilePath[length - 1] == 's')
		{
			return Z_SHADER_VERTEX;
		}
		else if (resourceFilePath[length - 4] == 'f' && resourceFilePath[length - 3] == 'r' && resourceFilePath[length - 2] == 'a' && resourceFilePath[length - 1] == 'g')
		{
			return Z_SHADER_PIXEL;
		}
		return 0;
	}

	ShaderManager* ShaderManager::s_instance = nullptr;

	void ShaderManager::Init(GameContext* _gameContext)
	{
		Handle handle("SHADER MANAGER", sizeof(ShaderManager));
		s_instance = new(handle) ShaderManager;
		s_instance->m_gameContext = _gameContext;

		s_instance->loadResourceAsync("ZooidEngine/Shaders/DefaultGLSimple.vs");
		s_instance->loadResourceAsync("ZooidEngine/Shaders/TestGLVertexShader.vs");
		s_instance->loadResourceAsync("ZooidEngine/Shaders/TestGLFragmentShader.frag");
		s_instance->loadResourceAsync("ZooidEngine/Shaders/DefaultGLSimple.frag");
		s_instance->loadResourceAsync("ZooidEngine/Shaders/DefaultGLSimpleColor.frag");
		s_instance->loadResourceAsync("ZooidEngine/Shaders/DefaultGLSimpleLit.vs");
		s_instance->loadResourceAsync("ZooidEngine/Shaders/DefaultGLSimpleLit.frag");

		s_instance->loadAllResource();


		_gameContext->getRenderer()->AcquireRenderThreadOwnership();

		{
			Handle hShaderChain = _gameContext->getRenderZooid()->CreateShaderChain();
			IShaderChain* shaderChain = hShaderChain.getObject<IShaderChain>();
			shaderChain->MakeChain(s_instance->getResource<IShader>("ZooidEngine/Shaders/TestGLVertexShader.vs"), s_instance->getResource<IShader>("ZooidEngine/Shaders/TestGLFragmentShader.frag"), nullptr, nullptr);
			s_instance->m_shaderChain.push_back(shaderChain);
		}

		{	
			Handle hShaderChain = _gameContext->getRenderZooid()->CreateShaderChain();
			IShaderChain* shaderChain = hShaderChain.getObject<IShaderChain>();
			shaderChain->MakeChain(s_instance->getResource<IShader>("ZooidEngine/Shaders/DefaultGLSimple.vs"), s_instance->getResource<IShader>("ZooidEngine/Shaders/DefaultGLSimple.frag"), nullptr, nullptr);
			s_instance->m_shaderChain.push_back(shaderChain);
		}

		{	
			Handle hShaderChain = _gameContext->getRenderZooid()->CreateShaderChain();
			IShaderChain* shaderChain = hShaderChain.getObject<IShaderChain>();
			shaderChain->MakeChain(s_instance->getResource<IShader>("ZooidEngine/Shaders/DefaultGLSimple.vs"), s_instance->getResource<IShader>("ZooidEngine/Shaders/DefaultGLSimpleColor.frag"), nullptr, nullptr);
			shaderChain->m_topology = TOPOLOGY_LINE;
			s_instance->m_shaderChain.push_back(shaderChain);
		}

		{
			Handle hShaderChain = _gameContext->getRenderZooid()->CreateShaderChain();
			IShaderChain* shaderChain = hShaderChain.getObject<IShaderChain>(); 
			shaderChain->MakeChain(s_instance->getResource<IShader>("ZooidEngine/Shaders/DefaultGLSimpleLit.vs"), s_instance->getResource<IShader>("ZooidEngine/Shaders/DefaultGLSimpleLit.frag"), nullptr, nullptr);
			s_instance->m_shaderChain.push_back(shaderChain);
		}


		_gameContext->getRenderer()->ReleaseRenderThreadOwnership();
	}

	void ShaderManager::Destroy()
	{
		for (int i = 0; i < s_instance->m_shaderChain.length(); i++)
		{
			s_instance->m_shaderChain[i]->Release();
		}

		if (s_instance)
		{
			s_instance->unloadResources();
		}
	}

	ZE::Handle ShaderManager::loadResource_Internal(const char* resourceFilePath)
	{

		m_gameContext->getRenderer()->AcquireRenderThreadOwnership();

		Handle hShader = m_gameContext->getRenderZooid()->CreateShader();
		IShader* pShader = hShader.getObject<IShader>();
		

		pShader->loadShader(resourceFilePath, getShaderTypeByName(resourceFilePath));
		
		m_gameContext->getRenderer()->ReleaseRenderThreadOwnership();
		
		return hShader;
	}

	void ShaderManager::preUnloadResource(Resource* _resource)
	{
		IShader* shader = _resource->m_hActual.getObject<IShader>();
		shader->release();
	}

	ZE::IShaderChain* ShaderManager::getShaderChain(int id)
	{
		return m_shaderChain[id];
	}

}
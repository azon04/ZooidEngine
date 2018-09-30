#include "ShaderManager.h"

#include "ZEGameContext.h"
#include "Renderer/RenderZooid.h"
#include "Renderer/IRenderer.h"

namespace ZE
{
	IMPLEMENT_CLASS_1(ShaderManager, ResourceManager)

	EShaderType ShaderManager::getShaderTypeByName(const char* resourceFilePath)
	{
		Int32 length = StringFunc::Length(resourceFilePath);

		if (resourceFilePath[length - 2] == 'v' && resourceFilePath[length - 1] == 's')
		{
			return SHADER_VERTEX;
		}
		else if (resourceFilePath[length - 4] == 'f' && resourceFilePath[length - 3] == 'r' && resourceFilePath[length - 2] == 'a' && resourceFilePath[length - 1] == 'g')
		{
			return SHADER_PIXEL;
		}
		return SHADER_MAX;
	}

	ShaderManager* ShaderManager::s_instance = nullptr;

	void ShaderManager::Init(GameContext* _gameContext)
	{
		Handle handle("SHADER MANAGER", sizeof(ShaderManager));
		s_instance = new(handle) ShaderManager;
		s_instance->m_gameContext = _gameContext;

		s_instance->loadResource("ZooidEngine/Shaders/DefaultGLSimple.vs");
		s_instance->loadResource("ZooidEngine/Shaders/TestGLVertexShader.vs");
		s_instance->loadResource("ZooidEngine/Shaders/TestGLFragmentShader.frag");
		s_instance->loadResource("ZooidEngine/Shaders/DefaultGLSimple.frag");
		s_instance->loadResource("ZooidEngine/Shaders/DefaultGLSimpleColor.frag");
		s_instance->loadResource("ZooidEngine/Shaders/DefaultGLSimpleLit.vs");
		s_instance->loadResource("ZooidEngine/Shaders/DefaultGLSimpleLit.frag");
		s_instance->loadResource("ZooidEngine/Shaders/DefaultGLSimpleLit_Blend.frag");
		s_instance->loadResource("ZooidEngine/Shaders/DefaultHighlightSimple.vs");
		s_instance->loadResource("ZooidEngine/Shaders/DefaultHighlightSimple.frag");
		s_instance->loadResource("ZooidEngine/Shaders/DefaultGLSimpleLitSkin.vs");
		s_instance->loadResource("ZooidEngine/Shaders/Shader2D/TextShader.vs");
		s_instance->loadResource("ZooidEngine/Shaders/Shader2D/TextShader.frag");
		s_instance->loadResource("ZooidEngine/Shaders/Shader2D/TextShader_SDF.frag");
		s_instance->loadResource("ZooidEngine/Shaders/Shader2D/TextShader_MSDF.frag");
		s_instance->loadResource("ZooidEngine/Shaders/TextWorldShader.vs");
		s_instance->loadResource("ZooidEngine/Shaders/TextWorldShader.frag");
		s_instance->loadResource("ZooidEngine/Shaders/TextWorldShader_SDF.frag");
		s_instance->loadResource("ZooidEngine/Shaders/TextWorldShader_MSDF.frag");
		s_instance->loadResource("ZooidEngine/Shaders/Shadows/DefaultDepthShader.vs");
		s_instance->loadResource("ZooidEngine/Shaders/Shadows/DefaultDepthShader.frag");
		s_instance->loadResource("ZooidEngine/Shaders/Shadows/DefaultSkinDepthShader.vs");
		s_instance->loadAllResource();


		_gameContext->getRenderer()->AcquireRenderThreadOwnership();

		// Z_SHADER_CHAIN_SIMPLE
		{
			Handle hShaderChain = _gameContext->getRenderZooid()->CreateShaderChain();
			IShaderChain* shaderChain = hShaderChain.getObject<IShaderChain>();
			shaderChain->makeChain(s_instance->getResource<IShader>("ZooidEngine/Shaders/TestGLVertexShader.vs"), s_instance->getResource<IShader>("ZooidEngine/Shaders/TestGLFragmentShader.frag"), nullptr, nullptr);
			s_instance->m_shaderChain.push_back(shaderChain);
		}

		// Z_SHADER_CHAIN_3D_DEFAULT
		{	
			Handle hShaderChain = _gameContext->getRenderZooid()->CreateShaderChain();
			IShaderChain* shaderChain = hShaderChain.getObject<IShaderChain>();
			shaderChain->makeChain(s_instance->getResource<IShader>("ZooidEngine/Shaders/DefaultGLSimple.vs"), s_instance->getResource<IShader>("ZooidEngine/Shaders/DefaultGLSimple.frag"), nullptr, nullptr);
			s_instance->m_shaderChain.push_back(shaderChain);
		}

		// Z_SHADER_CHAIN_3D_DEFAULT_COLOR_LINE
		{	
			Handle hShaderChain = _gameContext->getRenderZooid()->CreateShaderChain();
			IShaderChain* shaderChain = hShaderChain.getObject<IShaderChain>();
			shaderChain->makeChain(s_instance->getResource<IShader>("ZooidEngine/Shaders/DefaultGLSimple.vs"), s_instance->getResource<IShader>("ZooidEngine/Shaders/DefaultGLSimpleColor.frag"), nullptr, nullptr);
			shaderChain->m_topology = TOPOLOGY_LINE;
			s_instance->m_shaderChain.push_back(shaderChain);
		}

		// Z_SHADER_CHAIN_3D_DEFAULT_LIT
		{
			Handle hShaderChain = _gameContext->getRenderZooid()->CreateShaderChain();
			IShaderChain* shaderChain = hShaderChain.getObject<IShaderChain>(); 
			shaderChain->makeChain(s_instance->getResource<IShader>("ZooidEngine/Shaders/DefaultGLSimpleLit.vs"), s_instance->getResource<IShader>("ZooidEngine/Shaders/DefaultGLSimpleLit.frag"), nullptr, nullptr);
			s_instance->m_shaderChain.push_back(shaderChain);
		}

		// Z_SHADER_CHAIN_3D_DEFAULT_LIT_BLEND
		{
			Handle hShaderChain = _gameContext->getRenderZooid()->CreateShaderChain();
			IShaderChain* shaderChain = hShaderChain.getObject<IShaderChain>();
			shaderChain->makeChain(s_instance->getResource<IShader>("ZooidEngine/Shaders/DefaultGLSimpleLit.vs"), s_instance->getResource<IShader>("ZooidEngine/Shaders/DefaultGLSimpleLit_Blend.frag"), nullptr, nullptr);
			s_instance->m_shaderChain.push_back(shaderChain);
		}

		// Z_SHADER_CHAIN_3D_HIGHLIGHT
		{
			Handle hShaderChain = _gameContext->getRenderZooid()->CreateShaderChain();
			IShaderChain* shaderChain = hShaderChain.getObject<IShaderChain>();
			shaderChain->makeChain(s_instance->getResource<IShader>("ZooidEngine/Shaders/DefaultHighlightSimple.vs"), s_instance->getResource<IShader>("ZooidEngine/Shaders/DefaultHighlightSimple.frag"), nullptr, nullptr);
			s_instance->m_shaderChain.push_back(shaderChain);
		}

		// Z_SHADER_CHAIN_3D_DEFAULT_SKIN_LIT
		{
			Handle hShaderChain = _gameContext->getRenderZooid()->CreateShaderChain();
			IShaderChain* shaderChain = hShaderChain.getObject<IShaderChain>();
			shaderChain->makeChain(s_instance->getResource<IShader>("ZooidEngine/Shaders/DefaultGLSimpleLitSkin.vs"), s_instance->getResource<IShader>("ZooidEngine/Shaders/DefaultGLSimpleLit.frag"), nullptr, nullptr);
			s_instance->m_shaderChain.push_back(shaderChain);
		}

		// Z_SHADER_CHAIN_3D_DEFAULT_SKIN_LIT_BLEND
		{
			Handle hShaderChain = _gameContext->getRenderZooid()->CreateShaderChain();
			IShaderChain* shaderChain = hShaderChain.getObject<IShaderChain>();
			shaderChain->makeChain(s_instance->getResource<IShader>("ZooidEngine/Shaders/DefaultGLSimpleLitSkin.vs"), s_instance->getResource<IShader>("ZooidEngine/Shaders/DefaultGLSimpleLit_Blend.frag"), nullptr, nullptr);
			s_instance->m_shaderChain.push_back(shaderChain);
		}

		// Z_SHADER_CHAIN_WORLD_TEXT_SHADER
		{
			Handle hShaderChain = _gameContext->getRenderZooid()->CreateShaderChain();
			IShaderChain* shaderChain = hShaderChain.getObject<IShaderChain>();
			shaderChain->makeChain(s_instance->getResource<IShader>("ZooidEngine/Shaders/TextWorldShader.vs"), s_instance->getResource<IShader>("ZooidEngine/Shaders/TextWorldShader.frag"), nullptr, nullptr);
			s_instance->m_shaderChain.push_back(shaderChain);
		}

		// Z_SHADER_CHAIN_WORLD_TEXT_SHADER_SDF
		{
			Handle hShaderChain = _gameContext->getRenderZooid()->CreateShaderChain();
			IShaderChain* shaderChain = hShaderChain.getObject<IShaderChain>();
			shaderChain->makeChain(s_instance->getResource<IShader>("ZooidEngine/Shaders/TextWorldShader.vs"), s_instance->getResource<IShader>("ZooidEngine/Shaders/TextWorldShader_SDF.frag"), nullptr, nullptr);
			s_instance->m_shaderChain.push_back(shaderChain);
		}

		// Z_SHADER_CHAIN_WORLD_TEXT_SHADER_MSDF
		{
			Handle hShaderChain = _gameContext->getRenderZooid()->CreateShaderChain();
			IShaderChain* shaderChain = hShaderChain.getObject<IShaderChain>();
			shaderChain->makeChain(s_instance->getResource<IShader>("ZooidEngine/Shaders/TextWorldShader.vs"), s_instance->getResource<IShader>("ZooidEngine/Shaders/TextWorldShader_MSDF.frag"), nullptr, nullptr);
			s_instance->m_shaderChain.push_back(shaderChain);
		}

		// Z_SHADER_CHAIN_SCREEN_TEXT_SHADER
		{
			Handle hShaderChain = _gameContext->getRenderZooid()->CreateShaderChain();
			IShaderChain* shaderChain = hShaderChain.getObject<IShaderChain>();
			shaderChain->makeChain(s_instance->getResource<IShader>("ZooidEngine/Shaders/Shader2D/TextShader.vs"), s_instance->getResource<IShader>("ZooidEngine/Shaders/Shader2D/TextShader.frag"), nullptr, nullptr);
			s_instance->m_shaderChain.push_back(shaderChain);
		}

		// Z_SHADER_CHAIN_SCREEN_TEXT_SHADER_SDF
		{
			Handle hShaderChain = _gameContext->getRenderZooid()->CreateShaderChain();
			IShaderChain* shaderChain = hShaderChain.getObject<IShaderChain>();
			shaderChain->makeChain(s_instance->getResource<IShader>("ZooidEngine/Shaders/Shader2D/TextShader.vs"), s_instance->getResource<IShader>("ZooidEngine/Shaders/Shader2D/TextShader_SDF.frag"), nullptr, nullptr);
			s_instance->m_shaderChain.push_back(shaderChain);
		}

		// Z_SHADER_CHAIN_SCREEN_TEXT_SHADER_MSDF
		{
			Handle hShaderChain = _gameContext->getRenderZooid()->CreateShaderChain();
			IShaderChain* shaderChain = hShaderChain.getObject<IShaderChain>();
			shaderChain->makeChain(s_instance->getResource<IShader>("ZooidEngine/Shaders/Shader2D/TextShader.vs"), s_instance->getResource<IShader>("ZooidEngine/Shaders/Shader2D/TextShader_MSDF.frag"), nullptr, nullptr);
			s_instance->m_shaderChain.push_back(shaderChain);
		}

		// Z_SHADER_CHAIN_SHADOW_DEPTH
		{
			Handle hShaderChain = _gameContext->getRenderZooid()->CreateShaderChain();
			IShaderChain* shaderChain = hShaderChain.getObject<IShaderChain>();
			shaderChain->makeChain(s_instance->getResource<IShader>("ZooidEngine/Shaders/Shadows/DefaultDepthShader.vs"), s_instance->getResource<IShader>("ZooidEngine/Shaders/Shadows/DefaultDepthShader.frag"), nullptr, nullptr);
			s_instance->m_shaderChain.push_back(shaderChain);
		}

		// Z_SHADER_CHAIN_SHADOW_DEPTH_SKINNED
		{
			Handle hShaderChain = _gameContext->getRenderZooid()->CreateShaderChain();
			IShaderChain* shaderChain = hShaderChain.getObject<IShaderChain>();
			shaderChain->makeChain(s_instance->getResource<IShader>("ZooidEngine/Shaders/Shadows/DefaultSkinDepthShader.vs"), s_instance->getResource<IShader>("ZooidEngine/Shaders/Shadows/DefaultDepthShader.frag"), nullptr, nullptr);
			s_instance->m_shaderChain.push_back(shaderChain);
		}


		_gameContext->getRenderer()->ReleaseRenderThreadOwnership();
	}

	void ShaderManager::Destroy()
	{
		for (int i = 0; i < s_instance->m_shaderChain.length(); i++)
		{
			s_instance->m_shaderChain[i]->release();
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
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

		// Z_SHADER_CHAIN_SIMPLE
		s_instance->makeShaderChain("ZooidEngine/Shaders/TestGLVertexShader.vs", "ZooidEngine/Shaders/TestGLFragmentShader.frag", nullptr, nullptr);

		// Z_SHADER_CHAIN_3D_DEFAULT
		s_instance->makeShaderChain("ZooidEngine/Shaders/DefaultGLSimple.vs", "ZooidEngine/Shaders/DefaultGLSimple.frag", nullptr, nullptr);

		// Z_SHADER_CHAIN_3D_DEFAULT_COLOR_LINE
		s_instance->makeShaderChain("ZooidEngine/Shaders/DefaultGLSimple.vs", "ZooidEngine/Shaders/DefaultGLSimpleColor.frag", nullptr, nullptr);

		// Z_SHADER_CHAIN_3D_DEFAULT_LIT
		s_instance->makeShaderChain("ZooidEngine/Shaders/DefaultGLSimpleLit.vs", "ZooidEngine/Shaders/DefaultGLSimpleLit.frag", nullptr, nullptr);

		// Z_SHADER_CHAIN_3D_DEFAULT_LIT_BLEND
		s_instance->makeShaderChain("ZooidEngine/Shaders/DefaultGLSimpleLit.vs", "ZooidEngine/Shaders/DefaultGLSimpleLit_Blend.frag", nullptr, nullptr);

		// Z_SHADER_CHAIN_3D_HIGHLIGHT
		s_instance->makeShaderChain("ZooidEngine/Shaders/DefaultHighlightSimple.vs", "ZooidEngine/Shaders/DefaultHighlightSimple.frag", nullptr, nullptr);

		// Z_SHADER_CHAIN_3D_DEFAULT_SKIN_LIT
		s_instance->makeShaderChain("ZooidEngine/Shaders/DefaultGLSimpleLitSkin.vs", "ZooidEngine/Shaders/DefaultGLSimpleLit.frag", nullptr, nullptr);

		// Z_SHADER_CHAIN_3D_DEFAULT_SKIN_LIT_BLEND
		s_instance->makeShaderChain("ZooidEngine/Shaders/DefaultGLSimpleLitSkin.vs", "ZooidEngine/Shaders/DefaultGLSimpleLit_Blend.frag", nullptr, nullptr);

		// Z_SHADER_CHAIN_WORLD_TEXT_SHADER
		s_instance->makeShaderChain("ZooidEngine/Shaders/TextWorldShader.vs", "ZooidEngine/Shaders/TextWorldShader.frag", nullptr, nullptr);

		// Z_SHADER_CHAIN_WORLD_TEXT_SHADER_SDF
		s_instance->makeShaderChain("ZooidEngine/Shaders/TextWorldShader.vs", "ZooidEngine/Shaders/TextWorldShader_SDF.frag", nullptr, nullptr);

		// Z_SHADER_CHAIN_WORLD_TEXT_SHADER_MSDF
		s_instance->makeShaderChain("ZooidEngine/Shaders/TextWorldShader.vs", "ZooidEngine/Shaders/TextWorldShader_MSDF.frag", nullptr, nullptr);

		// Z_SHADER_CHAIN_SCREEN_TEXT_SHADER
		s_instance->makeShaderChain("ZooidEngine/Shaders/Shader2D/TextShader.vs", "ZooidEngine/Shaders/Shader2D/TextShader.frag", nullptr, nullptr);

		// Z_SHADER_CHAIN_SCREEN_TEXT_SHADER_SDF
		s_instance->makeShaderChain("ZooidEngine/Shaders/Shader2D/TextShader.vs", "ZooidEngine/Shaders/Shader2D/TextShader_SDF.frag", nullptr, nullptr);

		// Z_SHADER_CHAIN_SCREEN_TEXT_SHADER_MSDF
		s_instance->makeShaderChain("ZooidEngine/Shaders/Shader2D/TextShader.vs", "ZooidEngine/Shaders/Shader2D/TextShader_MSDF.frag", nullptr, nullptr);

		// Z_SHADER_CHAIN_SHADOW_DEPTH
		s_instance->makeShaderChain("ZooidEngine/Shaders/Shadows/DefaultDepthShader.vs", "ZooidEngine/Shaders/Shadows/DefaultDepthShader.frag", nullptr, nullptr);

		// Z_SHADER_CHAIN_SHADOW_DEPTH_SKINNED
		s_instance->makeShaderChain("ZooidEngine/Shaders/Shadows/DefaultSkinDepthShader.vs", "ZooidEngine/Shaders/Shadows/DefaultDepthShader.frag", nullptr, nullptr);
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
		ScopedRenderThreadOwnership renderLock(m_gameContext->getRenderer());
		
		Handle hShader = m_gameContext->getRenderZooid()->CreateShader();
		IShader* pShader = hShader.getObject<IShader>();
		
		pShader->loadShader(resourceFilePath, getShaderTypeByName(resourceFilePath));
		
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

	IShaderChain* ShaderManager::makeShaderChain(const char* vertexShaderPath, const char* fragmentShaderPath, const char* geomShaderPath, const char* computeShaderPath)
	{
		ScopedRenderThreadOwnership renderLock(m_gameContext->getRenderer());
		Handle hShaderChain = m_gameContext->getRenderZooid()->CreateShaderChain();
		IShaderChain* shaderChain = hShaderChain.getObject<IShaderChain>();
		shaderChain->makeChain(
			vertexShaderPath ? s_instance->loadResource<IShader>(vertexShaderPath) : nullptr,
			fragmentShaderPath ? s_instance->loadResource<IShader>(fragmentShaderPath) : nullptr,
			geomShaderPath ? s_instance->loadResource<IShader>(geomShaderPath) : nullptr,
			computeShaderPath ? s_instance->loadResource<IShader>(computeShaderPath) : nullptr);
		m_shaderChain.push_back(shaderChain);
		return shaderChain;
	}

}
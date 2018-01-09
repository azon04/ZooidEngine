#include "ShaderManager.h"

namespace ZE
{
	IMPLEMENT_CLASS_1(ShaderManager, ResourceManager)

	UInt8 ShaderManager::getShaderTypeByName(const char* resourceFilePath)
	{
		Int32 length = StringFunc::Length(resourceFilePath);

		if (resourceFilePath[length - 3] == 'v' && resourceFilePath[length - 2] == 's')
		{
			return Z_SHADER_VERTEX;
		}
		else if (resourceFilePath[length - 5] == 'f' && resourceFilePath[length - 4] == 'r' && resourceFilePath[length - 3] == 'a' && resourceFilePath[length - 2] == 'g')
		{
			return Z_SHADER_PIXEL;
		}
		return 0;
	}

	ShaderManager* ShaderManager::s_instance = nullptr;

	void ShaderManager::Init()
	{
		Handle handle("SHADER MANAGER", sizeof(ShaderManager));
		s_instance = new(handle) ShaderManager;

		s_instance->loadResourceAsync("ZooidEngine/Shaders/DefaultGLSimple.vs");
		s_instance->loadResourceAsync("ZooidEngine/Shaders/TestGLVertexShader.vs");
		s_instance->loadResourceAsync("ZooidEngine/Shaders/TestGLFragmentShader.frag");
		s_instance->loadResourceAsync("ZooidEngine/Shaders/DefaultGLSimple.frag");
		s_instance->loadResourceAsync("ZooidEngine/Shaders/DefaultGLSimpleColor.frag");
		s_instance->loadResourceAsync("ZooidEngine/Shaders/DefaultGLSimpleLit.vs");
		s_instance->loadResourceAsync("ZooidEngine/Shaders/DefaultGLSimpleLit.frag");

		s_instance->loadAllResource();

		{
			Handle hShaderChain(sizeof(ShaderChain));
			ShaderChain* shaderChain = new(hShaderChain) ShaderChain();
			shaderChain->MakeChain(s_instance->getResource<Shader>("ZooidEngine/Shaders/TestGLVertexShader.vs"), s_instance->getResource<Shader>("ZooidEngine/Shaders/TestGLFragmentShader.frag"), nullptr, nullptr);
			s_instance->m_shaderChain.push_back(shaderChain);
		}

		{
			Handle hShaderChain(sizeof(ShaderChain));
			ShaderChain* shaderChain = new(hShaderChain) ShaderChain();
			shaderChain->MakeChain(s_instance->getResource<Shader>("ZooidEngine/Shaders/DefaultGLSimple.vs"), s_instance->getResource<Shader>("ZooidEngine/Shaders/DefaultGLSimple.frag"), nullptr, nullptr);
			s_instance->m_shaderChain.push_back(shaderChain);
		}

		{
			Handle hShaderChain(sizeof(ShaderChain));
			ShaderChain* shaderChain = new(hShaderChain) ShaderChain();
			shaderChain->MakeChain(s_instance->getResource<Shader>("ZooidEngine/Shaders/DefaultGLSimple.vs"), s_instance->getResource<Shader>("ZooidEngine/Shaders/DefaultGLSimpleColor.frag"), nullptr, nullptr);
			shaderChain->m_topology = TOPOLOGY_LINE;
			s_instance->m_shaderChain.push_back(shaderChain);
		}

		{
			Handle hShaderChain(sizeof(ShaderChain));
			ShaderChain* shaderChain = new(hShaderChain) ShaderChain();
			shaderChain->MakeChain(s_instance->getResource<Shader>("ZooidEngine/Shaders/DefaultGLSimpleLit.vs"), s_instance->getResource<Shader>("ZooidEngine/Shaders/DefaultGLSimpleLit.frag"), nullptr, nullptr);
			s_instance->m_shaderChain.push_back(shaderChain);
		}
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
		Handle hShader(sizeof(Shader));
		Shader* pShader = new(hShader) Shader();
		pShader->loadShader(resourceFilePath, getShaderTypeByName(resourceFilePath));
		return hShader;
	}

	void ShaderManager::preUnloadResource(Resource* _resource)
	{
		Shader* shader = _resource->m_hActual.getObject<Shader>();
		shader->release();
	}

	ZE::ShaderChain* ShaderManager::getShaderChain(int id)
	{
		return m_shaderChain[id];
	}

}
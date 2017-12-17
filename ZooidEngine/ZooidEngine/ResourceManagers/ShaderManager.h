#ifndef __ZE_SHADER_MANAGER_H__
#define __ZE_SHADER_MANAGER_H__

#include "ResourceManager.h"
#include "Renderer/Shader.h"

namespace ZE
{
	class ShaderManager : public ResourceManager
	{
	public:
		ShaderManager() {}

		static void Init();
		static void Destroy();
		static ShaderManager* getInstance() { return s_instance; }
		
		virtual Handle loadResource(const char* resourceFilePath);
		virtual void preUnloadResource(Resource* _resource);

		ShaderChain* getShaderChain(int id);
	protected:

		UInt8 getShaderTypeByName(const char* resourceFilePath);

		static ShaderManager* s_instance;

		Array<ShaderChain*, true> m_shaderChain;

	};
};

#endif

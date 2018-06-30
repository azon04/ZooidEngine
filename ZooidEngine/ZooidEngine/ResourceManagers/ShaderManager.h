#ifndef __ZE_SHADER_MANAGER_H__
#define __ZE_SHADER_MANAGER_H__

#include "ResourceManager.h"
#include "Renderer/IShader.h"

namespace ZE
{
	class GameContext;

	class ShaderManager : public ResourceManager
	{
		DEFINE_CLASS(ShaderManager)

	public:
		ShaderManager() {}

		static void Init(GameContext* _gameContext);
		static void Destroy();
		static ShaderManager* getInstance() { return s_instance; }
		
		virtual Handle loadResource_Internal(const char* resourceFilePath);
		virtual void preUnloadResource(Resource* _resource);

		IShaderChain* getShaderChain(int id);

		GameContext* m_gameContext;
	protected:

		ShaderType getShaderTypeByName(const char* resourceFilePath);

		static ShaderManager* s_instance;

		Array<IShaderChain*, true> m_shaderChain;

	};
};

#endif

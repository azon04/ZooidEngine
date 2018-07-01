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

		static void Init(GameContext* _gameContext);
		static void Destroy();
		static ShaderManager* GetInstance() { return s_instance; }
		
		IShaderChain* getShaderChain(int id);

	protected:
		// ResourceManager implementation
		virtual Handle loadResource_Internal(const char* resourceFilePath) override;
		virtual void preUnloadResource(Resource* _resource) override;

		ShaderType getShaderTypeByName(const char* resourceFilePath);


	protected:
		static ShaderManager* s_instance;

		Array<IShaderChain*, true> m_shaderChain;
		GameContext* m_gameContext;

	};
};

#endif

#ifndef __ZE_EM_TO_IRRADIANCE_H__
#define __ZE_EM_TO_IRRADIANCE_H__

#include "../RenderPass.h"
#include "Common/SingletonClass.h"

namespace ZE
{
	class IGPUTexture;
	class IGPURenderBuffer;
	class IGPUFrameBuffer;
	class GameContext;
	class IShaderChain;

	class EMToIrradianceMap : public RenderPassBase, public Singleton<EMToIrradianceMap>
	{
	public:
		virtual void prepare(GameContext* _gameContext);
		virtual void release(GameContext* _gameContext);

		virtual bool execute_CPU(GameContext* _gameContext);
		virtual bool execute_GPU(GameContext* _gameContext);

		static Handle ConvertToIrradianceMap(GameContext* _gameContext, IGPUTexture* environmentMap);

	private:
		IGPUTexture* m_environmentMap;
		IShaderChain* m_shaderChain = nullptr;
		IGPUFrameBuffer* m_frameBuffer = nullptr;
		IGPURenderBuffer* m_depthBuffer = nullptr;
		Handle m_result;

		Int32 m_textureSize = 32;
	};
}
#endif

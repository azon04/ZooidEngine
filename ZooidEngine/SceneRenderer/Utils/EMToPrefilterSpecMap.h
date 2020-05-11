#ifndef __ZE_EM_TO_PREFILTER_SPECULAR_MAP_H__
#define __ZE_EM_TO_PREFILTER_SPECULAR_MAP_H__

#include "../RenderPass.h"
#include "Common/SingletonClass.h"

namespace ZE
{

	class IGPUTexture;
	class IGPURenderBuffer;
	class IGPUFrameBuffer;
	class GameContext;
	class IShaderChain;

	class EMToPrefilterSpecMap : public RenderPassBase, public Singleton<EMToPrefilterSpecMap>
	{
	public:
		virtual void prepare(GameContext* _gameContext);
		virtual void release(GameContext* _gameContext);

		virtual bool execute_CPU(GameContext* _gameContext) { return true; }
		virtual bool execute_GPU(GameContext* _gameContext);

		static Handle ConvertToPrefilterSpecMap(GameContext* _gameContext, IGPUTexture* environmentMap);

	private:
		IGPUTexture* m_environmentMap;
		IShaderChain* m_shaderChain = nullptr;
		IGPUFrameBuffer* m_frameBuffer = nullptr;
		IGPURenderBuffer* m_depthBuffer = nullptr;
		Handle m_result;

		Int32 m_textureSize = 128;
	};
}
#endif

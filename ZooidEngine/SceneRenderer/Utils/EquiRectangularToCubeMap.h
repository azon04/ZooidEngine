#ifndef __ZE_EQUIRECTANGULAR_TO_CUBE_H__
#define __ZE_EQUIRECTANGULAR_TO_CUBE_H__

#include "../RenderPass.h"
#include "Common/SingletonClass.h"

namespace ZE
{
	class IGPUTexture;
	class IGPUFrameBuffer;
	class GameContext;
	class IShaderChain;

	class EquiRectangularToCubeMap : public RenderPassBase, public Singleton<EquiRectangularToCubeMap>
	{
	public:
		virtual void prepare(GameContext* _gameContext);
		virtual void release(GameContext* _gameContext);

		virtual bool execute_CPU(GameContext* _gameContext);
		virtual bool execute_GPU(GameContext* _gameContext);

		static Handle ConvertToCubeMap(GameContext* _gameContext, IGPUTexture* equiRectangularTexture);

	private:
		IGPUTexture* m_equiRectangularTexture;
		IShaderChain* m_shaderChain = nullptr;
		Handle m_result;
	};
}
#endif

#ifndef __ZE_BASE_FORWARD_RENDERGRAPH_H__
#define __ZE_BASE_FORWARD_RENDERGRAPH_H__

#include "../RenderGraph.h"
#include "Common/SingletonClass.h"

namespace ZE
{
	class BaseForwardRenderGraph : public RenderGraph, public Singleton<BaseForwardRenderGraph>
	{
	public:
		virtual void init(GameContext* _gameContext) override;
		virtual void release(GameContext* _gameContext) override;

		virtual void begin(GameContext* _gameContext) override;
		virtual bool execute(GameContext* _gameContext) override;
		virtual void end(GameContext* _gameContext) override;
	};
}

#endif

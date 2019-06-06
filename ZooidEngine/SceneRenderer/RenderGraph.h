#ifndef __ZE_RENDER_GRAPH_H__
#define __ZE_RENDER_GRAPH_H__

namespace ZE
{
	class GameContext;

	class RenderGraph
	{
	public:
		
		virtual void init(GameContext* _gameContext) = 0;
		virtual void release(GameContext* _gameContext) = 0;

		virtual void begin(GameContext* _gameContext) = 0;
		virtual bool execute(GameContext* _gameContext) = 0;
		virtual void end(GameContext* _gameContext) = 0;
	};
}
#endif

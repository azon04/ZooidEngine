#ifndef __ZOOID__
#define __ZOOID__

#include "ZEGameContext.h"

namespace ZE
{
	class BaseZooid
	{
	public:
		BaseZooid(GameContext *gameContext) 
			: m_gameContext(gameContext)
		{}

		virtual void Init() = 0;
		virtual void Destroy() = 0;

		GameContext* m_gameContext;
	};
}
#endif

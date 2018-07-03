#ifndef __ZOOID__
#define __ZOOID__

#include "ZEGameContext.h"

namespace ZE
{
	// Base Zooid (Zooid = Module)
	class BaseZooid
	{
	public:
		BaseZooid(GameContext *gameContext) 
			: m_gameContext(gameContext)
		{}

		virtual void Init() = 0;
		virtual void Destroy() = 0;

	protected:
		GameContext* m_gameContext;
	};
}
#endif

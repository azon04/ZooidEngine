#include "ZEGameContext.h"

namespace ZE 
{
	GameContext* gGameContext = nullptr;

	ZE::GameContext* GetGameContext()
	{
		return gGameContext;
	}

}
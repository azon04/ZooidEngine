#include "ZEGameContext.h"

namespace ZE 
{
	GameContext* gGameContext = nullptr;

	Int32 gRenderWidth = 1600;
	Int32 gRenderHeight = 900;

	ZE::GameContext* GetGameContext()
	{
		return gGameContext;
	}

}
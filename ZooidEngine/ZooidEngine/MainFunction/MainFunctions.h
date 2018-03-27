#ifndef __ZE_MAIN_FUNCTIONS__ 
#define __ZE_MAIN_FUNCTIONS__

#include "ZEGameContext.h"

#ifndef ZE_RENDER_MULTITHREAD
#define ZE_RENDER_MULTITHREAD 1
#endif

namespace ZE {
	void MainSetup(GameContext* _gameContext);
	void MainClean(GameContext* _gameContext);

	void MainThreadJob(GameContext* _gameContext);
	void MainDrawJob(GameContext* _gameContext);
	void DrawJob(GameContext* _gameContext);

	extern ConditionVariable g_drawThreadVariable;
	extern Mutex g_drawMutex;
	extern bool g_drawReady;
}
#endif // 

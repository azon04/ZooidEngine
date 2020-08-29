#ifndef __ZE_MAIN_FUNCTIONS__ 
#define __ZE_MAIN_FUNCTIONS__

#include "ZEGameContext.h"

namespace ZE 
{
	void MainSetup(GameContext* _gameContext);
	void MainClean(GameContext* _gameContext);

	void MainThreadJob(GameContext* _gameContext);
	void MainDrawJob(GameContext* _gameContext);
	void DrawJob(GameContext* _gameContext);

	extern ConditionVariable g_drawThreadVariable;
}
#endif 

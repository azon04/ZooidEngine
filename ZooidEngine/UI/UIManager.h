#ifndef __ZE_UI_MANAGER_H__
#define __ZE_UI_MANAGER_H__

#include "ZooidUI.h"

namespace ZE
{
	class GameContext;
	class UIManager
	{

	protected:
		UIManager();
		~UIManager();

	public:
		static void Init(GameContext* _gameContext);
		static void Destroy();

	private:
		static UIManager* s_instance;
	};
}
#endif

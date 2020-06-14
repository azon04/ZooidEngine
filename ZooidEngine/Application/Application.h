#ifndef __ZE_APPLICATION_H__
#define __ZE_APPLICATION_H__

namespace ZE
{
	class GameContext;

	// Application class to create per project setup
	class Application
	{
	public:
		virtual void Setup(GameContext* _gameContext) = 0;
		virtual void Tick(GameContext* _gameContext, float deltaTime) = 0;
		virtual void Clean(GameContext* _gameContext) = 0;

		static Application* GetApplication();
	};

	class EmptyApplication : public Application
	{
		virtual void Setup(GameContext* _gameContext) override {}
		virtual void Tick(GameContext* _gameContext, float deltaTime) override {}
		virtual void Clean(GameContext* _gameContext) override {}
	};
}

#endif

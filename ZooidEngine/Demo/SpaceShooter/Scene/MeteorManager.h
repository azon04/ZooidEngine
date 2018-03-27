#pragma once

#include "ZooidEngine/Scene/SceneComponent.h"

namespace SpaceShooter
{
	class MeteorManager : public ZE::SceneComponent
	{
		DEFINE_CLASS(MeteorManager)
	
	public:

		MeteorManager(ZE::GameContext* _gameContext) 
			: ZE::SceneComponent(_gameContext),
			m_delay(10.0f),
			m_timeSpawn(10.0f)
		{}

		virtual ~MeteorManager() {}
		
		virtual void handleUpdateEvent(ZE::Event* event) override;

		float m_timeSpawn;
		float m_delay;
	};
}

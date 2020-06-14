#include "MeteorManager.h"

#include "ZooidEngine/Events/Events.h"
#include "ZooidEngine/Memory/Handle.h"
#include "ZooidEngine/ZEGameContext.h"

#include "Meteor.h"

#include <cstdlib>

namespace ZE
{
	IMPLEMENT_CLASS_1(MeteorManager, ZE::SceneComponent)

	void MeteorManager::handleUpdateEvent(ZE::Event* event)
	{
		ZE::Event_UPDATE* eventUpdate = (ZE::Event_UPDATE*) event;

		m_delay -= eventUpdate->m_deltaSeconds;
		if (m_delay < 0.0f)
		{
			// Spawn Meteor
			ZE::Handle h("Meteor", sizeof(Meteor));
			float posY = (rand() % 100) - 50.0f;
			Meteor* pMeteor = new(h) Meteor(m_gameContext, Vector3(-10.0f, 0.0f, 0.0f), Vector3(150.0f, posY, 0.0f));
			pMeteor->setupComponent();
			addChild(pMeteor);

			m_delay += m_timeSpawn;
		}
	}

}
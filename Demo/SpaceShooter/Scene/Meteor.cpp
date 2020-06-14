#include "Meteor.h"

#include "ZooidEngine/Events/Events.h"
#include "ZooidEngine/FileSystem/DirectoryHelper.h"
#include "ZooidEngine/Memory/MemoryManager.h"

namespace ZE
{
	IMPLEMENT_CLASS_1(Meteor, ZE::RenderComponent)

	Meteor::Meteor(ZE::GameContext* _gameContext, Vector3 velocity, Vector3 initialPosition)
		: ZE::RenderComponent(_gameContext), m_velocity(velocity)
	{
		m_worldTransform.setPos(initialPosition);
	}

	void Meteor::setupComponent()
	{
		ZE::RenderComponent::setupComponent();
		loadMeshFromFile(ZE::GetResourcePath("SpaceShooter/Mesh/Sphere01.meshz").c_str());
		m_worldTransform.scale(Vector3(0.25f, 0.25f, 0.25f));
	}

	void Meteor::handleUpdateEvent(ZE::Event* event)
	{
		ZE::Event_UPDATE* updateEvent = (ZE::Event_UPDATE*) event;
		m_worldTransform.setPos(m_worldTransform.getPos() + m_velocity * updateEvent->m_deltaSeconds);
		//m_worldTransform.rotateAroundN(DegToRad( 90.0f * updateEvent->m_deltaTime / 1000.0f));
		
		if (m_worldTransform.getPos().getX() < -20)
		{
			removeFromParents();
			ZE::MemoryManager::GetInstance()->freeBlockAtAddress(this);
		}
	}

}
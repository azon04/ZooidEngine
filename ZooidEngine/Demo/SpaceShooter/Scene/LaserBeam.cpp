#include "LaserBeam.h"

#include "ZooidEngine/FileSystem/DirectoryHelper.h"
#include "ZooidEngine/Events/Events.h"

namespace SpaceShooter
{
	IMPLEMENT_CLASS_1(LaserBeam, ZE::RenderComponent);

	LaserBeam::LaserBeam(ZE::GameContext* _gameContext, Vector3 velocity, Vector3 initialPosition) :
		ZE::RenderComponent(_gameContext),
		m_velocity(velocity)
	{
		m_worldTransform.setPos(initialPosition);
	}

	void LaserBeam::setupComponent()
	{
		ZE::RenderComponent::setupComponent();

		this->fromFile(ZE::GetResourcePath("SpaceShooter/Mesh/Cylinder.meshz").c_str());
	}

	void LaserBeam::handleUpdateEvent(ZE::Event* event)
	{
		ZE::RenderComponent::handleUpdateEvent(event);

		ZE::Event_UPDATE* eventUpdate = (ZE::Event_UPDATE*) event;
		m_worldTransform.setPos(m_worldTransform.getPos() + (m_velocity * eventUpdate->m_deltaSeconds));
		
		if (m_worldTransform.getPos().getX() > 500)
		{
			removeFromParents();
			ZE::MemoryManager::getInstance()->freeBlockAtAddress(this);
		}
	}

}
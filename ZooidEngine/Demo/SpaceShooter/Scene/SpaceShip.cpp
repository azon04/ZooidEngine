#include "SpaceShip.h"
#include "LaserBeam.h"

#include "ZooidEngine/Events/InputEvents.h"
#include "ZooidEngine/FileSystem/DirectoryHelper.h"
#include "ZooidEngine/Memory/Handle.h"
#include "ZooidEngine/ZEGameContext.h"

#include <windows.h>

namespace SpaceShooter
{
	IMPLEMENT_CLASS_1(SpaceShip, ZE::RenderComponent);

	void SpaceShip::setupComponent()
	{

		this->fromFile(ZE::GetResourcePath("SpaceShooter/Mesh/Starship.meshz").c_str());

		ZE::RenderComponent::setupComponent();
		addEventDelegate(ZE::Event_KEY_DOWN, &SpaceShooter::SpaceShip::handleMovementInput);
		addEventDelegate(ZE::Event_KEY_UP, &SpaceShooter::SpaceShip::handleMovementInput);
	}

	void SpaceShip::handleMovementInput(ZE::Event* evt)
	{
		float multiplier = -1.0f;
		ZE::Event_INPUT* pInput = (ZE::Event_INPUT*) evt;
		if (pInput->IsA(ZE::Event_KEY_DOWN::GetClassID()))
		{
			multiplier = 1.0f;
		}

		float speed = 10.0f;

		if (pInput->m_keyId == 'W')
		{
			m_movementSpeed = m_movementSpeed + Vector3(0.0f, multiplier * speed, 0.0f);
		}
		else if (pInput->m_keyId == 'S')
		{
			m_movementSpeed = m_movementSpeed + Vector3(0.0f, -1.0f * multiplier * speed, 0.0f);
		}
		else if (pInput->m_keyId == 'D')
		{
			m_movementSpeed = m_movementSpeed + Vector3(multiplier * speed, 0.0f, 0.0f);
		}
		else if (pInput->m_keyId == 'A')
		{
			m_movementSpeed = m_movementSpeed + Vector3(-1.0f * multiplier * speed, 0.0f, 0.0f);
		}
		else if (multiplier == -1.0f && pInput->m_keyId == VK_SPACE)
		{
			ZE::Handle h("LaserBeam", sizeof(LaserBeam));
			LaserBeam* pLaserBeam = new(h) LaserBeam(m_gameContext, m_worldTransform.getU() * 100.0f, m_worldTransform.getPos() + m_worldTransform.getU() * 10.0f);
			pLaserBeam->setupComponent();
			m_gameContext->getRootComponent()->addChild(pLaserBeam);
		}
	}

	void SpaceShip::handleUpdateEvent(ZE::Event* event)
	{
		ZE::RenderComponent::handleUpdateEvent(event);

		ZE::Event_UPDATE* eventUpdate = (ZE::Event_UPDATE*) event;
		float deltaTime = eventUpdate->m_deltaSeconds;
		m_worldTransform.setPos(m_worldTransform.getPos() + (m_movementSpeed * deltaTime));
	}

}
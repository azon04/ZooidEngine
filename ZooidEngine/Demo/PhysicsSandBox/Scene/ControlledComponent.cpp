#include "ControlledComponent.h"

#include "Events/InputEvents.h"
#include "FileSystem/DirectoryHelper.h"

#include "Physics/Physics.h"
#include "Physics/PhysicsBody.h"
#include "Physics/PhysicsBodyHolder.h"
#include "ZEGameContext.h"

#include "Logging/Log.h"

namespace PhysicsSandBox
{
	IMPLEMENT_CLASS_1(ControlledComponent, ZE::RenderComponent)

	void ControlledComponent::setupComponent()
	{
		addEventDelegate(ZE::Event_KEY_UP, &ControlledComponent::handleKeyUpEvent);
		addEventDelegate(ZE::Event_KEY_DOWN, &ControlledComponent::handleKeyDownEvent);

		fromFile(ZE::GetResourcePath("PhysicsSandBox/Mesh/Crate.meshz").c_str());

		m_bStatic = false;
		m_physicsEnabled = true;
		m_bTriggerOnly = true;
		m_bEnableGravity = false;

		RenderComponent::setupComponent();
	}

	void ControlledComponent::handleKeyUpEvent(ZE::Event* event)
	{
		ZE::Event_INPUT* pRealEvent = (ZE::Event_INPUT*)event;

		if (pRealEvent->m_keyId == 'Z')
		{
			ZE::PhysicsHit hit;

			if (m_gameContext->getPhysics()->DoLineRaycast(ZE::CollisionGroup::COLLISION_STATIC | ZE::CollisionGroup::COLLISION_DYNAMIC, m_worldTransform.getPos() - 1.2f * m_worldTransform.getN(), -1 * m_worldTransform.getN(), 20.0f, hit))
			{
				ZELOG(ZE::LOG_PHYSICS, ZE::Log, "Hit %s!", hit.BlockedComponent->getObjectName());
				
				if (hit.BlockedComponent)
				{
					ZE::IPhysicsBodyHolder* pPhysicsBodyHolder = dynamic_cast<ZE::IPhysicsBodyHolder*>(hit.BlockedComponent);
					pPhysicsBodyHolder->AddForceAtPos(hit.BlockedPosition, -1 * m_worldTransform.getN(), 10.0f, true);
				}
			}
		}
		else if (pRealEvent->m_keyId == 'X')
		{
			ZE::PhysicsHit hit;

			if (m_gameContext->getPhysics()->DoBoxCast(ZE::CollisionGroup::COLLISION_STATIC | ZE::CollisionGroup::COLLISION_DYNAMIC, m_worldTransform.getPos() - 1.2f * m_worldTransform.getN(), -1 * m_worldTransform.getN(), 20.0f, Quaternion(), Vector3(0.25f, 0.25f, 0.25f), hit))
			{
				ZELOG(ZE::LOG_PHYSICS, ZE::Log, "Hit %s!", hit.BlockedComponent->getObjectName());
			}
		}
		else if (pRealEvent->m_keyId == 'C')
		{
			ZE::PhysicsHit hit;

			if (m_gameContext->getPhysics()->DoCapsuleCast(ZE::CollisionGroup::COLLISION_STATIC | ZE::CollisionGroup::COLLISION_DYNAMIC, m_worldTransform.getPos() - 1.2f * m_worldTransform.getN(), -1 * m_worldTransform.getN(), 20.0f, Quaternion(), 0.45f, 0.45f, hit))
			{
				ZELOG(ZE::LOG_PHYSICS, ZE::Log, "Hit %s!", hit.BlockedComponent->getObjectName());
			}
		}
		else if (pRealEvent->m_keyId == 'V')
		{
			ZE::PhysicsHit hit;

			if (m_gameContext->getPhysics()->DoSphereCast(ZE::CollisionGroup::COLLISION_STATIC | ZE::CollisionGroup::COLLISION_DYNAMIC, m_worldTransform.getPos() - 1.2f * m_worldTransform.getN(), -1 * m_worldTransform.getN(), 20.0f, 0.45f, hit))
			{
				ZELOG(ZE::LOG_PHYSICS, ZE::Log, "Hit %s!", hit.BlockedComponent->getObjectName());
			}
		}
		else if (pRealEvent->m_keyId == 'B')
		{
			ZE::PhysicsHit hit;

			if (m_gameContext->getPhysics()->DoLineRaycastMulti(ZE::CollisionGroup::COLLISION_STATIC | ZE::CollisionGroup::COLLISION_DYNAMIC, m_worldTransform.getPos() - 1.2f * m_worldTransform.getN(), -1 * m_worldTransform.getN(), 20.0f, hit))
			{
				if (hit.BlockedComponent)
				{
					ZELOG(ZE::LOG_PHYSICS, ZE::Log, "Hit %s!", hit.BlockedComponent->getObjectName());
				}

				ZELOG(ZE::LOG_PHYSICS, ZE::Log, "Hit has %d touches", hit.Touches.size());
				for (int i = 0; i < hit.Touches.size(); i++)
				{
					ZE::PhysicsHit& touchHit = hit.Touches[i];
					ZELOG(ZE::LOG_PHYSICS, ZE::Log, "Touches Hit %s!", touchHit.BlockedComponent->getObjectName());
				}
			}
		}
		else if (pRealEvent->m_keyId == 'N')
		{
			ZE::PhysicsHit hit;

			if (m_gameContext->getPhysics()->DoBoxCastMulti(ZE::CollisionGroup::COLLISION_STATIC | ZE::CollisionGroup::COLLISION_DYNAMIC, m_worldTransform.getPos() - 1.2f * m_worldTransform.getN(), -1 * m_worldTransform.getN(), 20.0f, Quaternion(), Vector3(0.25f, 0.25f, 0.25f), hit))
			{
				if (hit.BlockedComponent)
				{
					ZELOG(ZE::LOG_PHYSICS, ZE::Log, "Hit %s!", hit.BlockedComponent->getObjectName());
				}

				ZELOG(ZE::LOG_PHYSICS, ZE::Log, "Hit has %d touches", hit.Touches.size());
				for (int i = 0; i < hit.Touches.size(); i++)
				{
					ZE::PhysicsHit& touchHit = hit.Touches[i];
					ZELOG(ZE::LOG_PHYSICS, ZE::Log, "Touches Hit %s!", touchHit.BlockedComponent->getObjectName());
				}
			}
		}
		else if (pRealEvent->m_keyId == 'M')
		{
			ZE::PhysicsHit hit;

			if (m_gameContext->getPhysics()->DoSphereCastMulti(ZE::CollisionGroup::COLLISION_STATIC | ZE::CollisionGroup::COLLISION_DYNAMIC, m_worldTransform.getPos() - 1.2f * m_worldTransform.getN(), -1 * m_worldTransform.getN(), 20.0f, 0.45f, hit))
			{
				if (hit.BlockedComponent)
				{
					ZELOG(ZE::LOG_PHYSICS, ZE::Log, "Hit %s!", hit.BlockedComponent->getObjectName());
				}

				ZELOG(ZE::LOG_PHYSICS, ZE::Log, "Hit has %d touches", hit.Touches.size());
				for (int i = 0; i < hit.Touches.size(); i++)
				{
					ZE::PhysicsHit& touchHit = hit.Touches[i];
					ZELOG(ZE::LOG_PHYSICS, ZE::Log, "Touches Hit %s!", touchHit.BlockedComponent->getObjectName());
				}
			}
		}
		else if (pRealEvent->m_keyId == 'K')
		{
			ZE::PhysicsHit hit;

			if (m_gameContext->getPhysics()->DoCapsuleCastMulti(ZE::CollisionGroup::COLLISION_STATIC | ZE::CollisionGroup::COLLISION_DYNAMIC, m_worldTransform.getPos() - 1.2f * m_worldTransform.getN(), -1 * m_worldTransform.getN(), 20.0f, Quaternion(), 0.45f, 0.45f, hit))
			{
				if (hit.BlockedComponent)
				{
					ZELOG(ZE::LOG_PHYSICS, ZE::Log, "Hit %s!", hit.BlockedComponent->getObjectName());
				}

				ZELOG(ZE::LOG_PHYSICS, ZE::Log, "Hit has %d touches", hit.Touches.size());
				for (int i = 0; i < hit.Touches.size(); i++)
				{
					ZE::PhysicsHit& touchHit = hit.Touches[i];
					ZELOG(ZE::LOG_PHYSICS, ZE::Log, "Touches Hit %s!", touchHit.BlockedComponent->getObjectName());
				}
			}
		}
	}

	void ControlledComponent::handleKeyDownEvent(ZE::Event* event)
	{

	}

}
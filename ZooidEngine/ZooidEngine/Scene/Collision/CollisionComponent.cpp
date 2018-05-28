#include "CollisionComponent.h"

#include "Physics/PhysicsEvents.h"
#include "Physics/PhysicsBody.h"
#include "Utils/Array.h"
#include "Logging/Log.h"

namespace ZE
{
	IMPLEMENT_CLASS_1(CollisionComponent, SceneComponent);

	void CollisionComponent::setupComponent()
	{
		SceneComponent::setupComponent();

		addEventDelegate(Event_Physics_UPDATE_TRANSFORM, &CollisionComponent::handlePhysicsUpdateTransform);
		addEventDelegate(Event_Physics_ONCOLLIDE, &CollisionComponent::handleOnCollide);
		addEventDelegate(Event_Physics_ON_BEGIN_TRIGGER, &CollisionComponent::handleOnBeginTrigger);
		addEventDelegate(Event_Physics_ON_END_TRIGGER, &CollisionComponent::handleOnEndTrigger);
		
		setupPhysics();
	}

	void CollisionComponent::setupPhysics()
	{

	}

	void CollisionComponent::handlePhysicsUpdateTransform(Event* pEvent)
	{
		Event_Physics_UPDATE_TRANSFORM* pRealEvent = static_cast<Event_Physics_UPDATE_TRANSFORM*>(pEvent);
		Matrix4x4 interMatrix = pRealEvent->m_worldTransform;
		interMatrix.scale(m_worldTransform.extractScale());
		m_worldTransform = interMatrix;
	}

	void CollisionComponent::handleOnCollide(Event* pEvent)
	{

	}

	void CollisionComponent::handleOnBeginTrigger(Event* pEvent)
	{
		ZELOG(LOG_PHYSICS, Log, "%s Begin Triggered!", getObjectName().c_str());
	}

	void CollisionComponent::handleOnEndTrigger(Event* pEvent)
	{
		ZELOG(LOG_PHYSICS, Log, "%s End Triggered!", getObjectName().c_str());
	}

}
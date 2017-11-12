#include "SceneComponent.h"

#include "Events/Events.h"

namespace ZE {

	IMPLEMENT_CLASS_1(SceneComponent, Component);

	void SceneComponent::calculateTransform(const Matrix4x4& parentMat)
	{
		m_worldTransform = m_transform * parentMat;
	}

	void SceneComponent::setupComponent()
	{
		Component::setupComponent();

		addEventDelegate(Event_UPDATE::GetClassID(), (EventDelegate::HandleEventFunc) &SceneComponent::handleUpdateEvent);
	}

	void SceneComponent::handleUpdateEvent(Event* event)
	{
		ZEINFO("UPDATE FUNCTION CALLED");
	}

}
#ifndef __ZE_SCENE_COMPONENT_H__
#define __ZE_SCENE_COMPONENT_H__

#include "../GameObjectModel/Component.h"
#include "../Math/Matrix4x4.h"

namespace ZE
{
	class Event;
	class SceneComponent : public Component {

		DEFINE_CLASS(SceneComponent)

	public:
		SceneComponent(GameContext* gameContext) : Component(gameContext) {}
		virtual ~SceneComponent() {}

		virtual void calculateTransform(const Matrix4x4& parentMat);
		virtual void setupComponent() override;

		Matrix4x4 m_transform;
		Matrix4x4 m_worldTransform;

	protected:


		void handleUpdateEvent(Event* event);

	};
}

#endif
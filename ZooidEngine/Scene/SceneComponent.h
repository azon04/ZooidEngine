#ifndef __ZE_SCENE_COMPONENT_H__
#define __ZE_SCENE_COMPONENT_H__

#include "GameObjectModel/Component.h"
#include "Math/Matrix4x4.h"

namespace ZE
{
	class Event;
	class SceneComponent : public Component 
	{

		DEFINE_CLASS(SceneComponent)

	public:
		SceneComponent(GameContext* gameContext) : Component(gameContext) {}
		virtual ~SceneComponent() {}

		virtual void calculateTransform(const Matrix4x4& parentMat);
		virtual void setupComponent() override;

		void setLocalTransform(Matrix4x4& _localTransform);
		void setWorldTransform(Matrix4x4& _worldTransform);

		void setWorldPosition(const Vector3& _worldPosition);
		void setRelativePosition(Vector3 _relativePosition);
		void setScale(Vector3 _scale);
		void rotateInDeg(Vector3 _eulerAngle);

		Vector3 getRelativePosition() const;
		Vector3 getWorldPosition() const;
		Vector3 getForwardVector() const;
		Vector3 getRightVector() const;
		Vector3 getUpVector() const;

		Matrix4x4 getLocalTransform();
		Matrix4x4 getWorldTransform();

	protected:

		virtual void handleUpdateEvent(Event* event);

		Matrix4x4 m_localTransform;
		Matrix4x4 m_worldTransform;
	};
}

#endif
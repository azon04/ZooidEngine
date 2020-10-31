#ifndef __ZE_SCENE_COMPONENT_H__
#define __ZE_SCENE_COMPONENT_H__

#include "GameObjectModel/Component.h"
#include "Math/Matrix4x4.h"
#include "Math/Transform.h"

namespace ZE
{
	class Event;
	class SceneComponent : public Component 
	{

		DEFINE_CLASS(SceneComponent)

	public:
		SceneComponent(GameContext* gameContext) : Component(gameContext), m_bTransformDirty(true) {}
		virtual ~SceneComponent() {}

		virtual void calculateTransform(const Matrix4x4& parentMat);
		virtual void setupComponent() override;

		void setLocalTransform(Matrix4x4& _localTransform);
		void setWorldTransform(Matrix4x4& _worldTransform);

		void setWorldPosition(const Vector3& _worldPosition);
		void setRelativePosition(const Vector3& _relativePosition);
		void setScale(const Vector3& _scale);
		void rotateInDeg(const Vector3& _eulerAngle);

		Vector3 getRelativePosition() const;
		Vector3 getWorldPosition() const;
		Vector3 getForwardVector() const;
		Vector3 getRightVector() const;
		Vector3 getUpVector() const;

		Matrix4x4 getLocalTransform();
		Matrix4x4 getWorldTransform();

	protected:

		virtual void handleUpdateEvent(Event* event);
		virtual void handleCalculateTransform(Event* event);
		virtual void updateCacheMatrix();

		ALIGN16 Matrix4x4 m_localTransform;
		ALIGN16 Matrix4x4 m_cacheWorldMatrix;

		Transform m_worldTransform;

		bool m_bTransformDirty:1;
	};
}

#endif
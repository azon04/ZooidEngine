#include "SceneComponent.h"

#include "Events/Events.h"

namespace ZE {

	IMPLEMENT_CLASS_1(SceneComponent, Component);

	void SceneComponent::calculateTransform(const Matrix4x4& parentMat)
	{
		Matrix4x4::FastMul(parentMat, m_localTransform, m_cacheWorldMatrix);
	}

	void SceneComponent::setupComponent()
	{
		Component::setupComponent();

		addEventDelegate(Event_UPDATE, &SceneComponent::handleUpdateEvent);
		addEventDelegate(Event_CALC_TRANSFORM, &SceneComponent::handleCalculateTransform);
	}

	void SceneComponent::setLocalTransform(Matrix4x4& _localTransform)
	{
		m_localTransform = _localTransform;

		// #TODO Calculate new world transform to its children
	}

	void SceneComponent::setWorldTransform(Matrix4x4& _worldTransform)
	{
		m_cacheWorldMatrix = _worldTransform;

		// #TODO Calculate new world transform to its children
	}

	void SceneComponent::setWorldPosition(const Vector3& _worldPosition)
	{
		m_worldTransform.setPosition(_worldPosition);
		m_bTransformDirty = true;

		// #TODO Calculate new world transform to its children
	}

	void SceneComponent::handleUpdateEvent(Event* event)
	{
	}

	void SceneComponent::handleCalculateTransform(Event* event)
	{
		updateCacheMatrix();
	}

	void SceneComponent::updateCacheMatrix()
	{
		if (m_bTransformDirty)
		{
			m_cacheWorldMatrix = m_worldTransform.toMatrix();
			m_bTransformDirty = false;
		}
	}

	void SceneComponent::setRelativePosition(const Vector3& _relativePosition)
	{
		m_localTransform.setPos(_relativePosition);

		// #TODO Calculate new world transform to its children
	}

	void SceneComponent::setScale(const Vector3& _scale)
	{
		m_worldTransform.setScale(_scale);
		m_bTransformDirty = true;

		// #TODO Calculate new world transform to its children
	}

	void SceneComponent::rotateInDeg(const Vector3& _eulerAngle)
	{
		m_worldTransform.setRotation(_eulerAngle);
		m_bTransformDirty = true;

		// #TODO Calculate new world transform to its children
	}

	Vector3 SceneComponent::getRelativePosition() const
	{
		return m_localTransform.getPos();
	}

	Vector3 SceneComponent::getWorldPosition() const
	{
		if (m_bTransformDirty)
		{
			return m_worldTransform.getPosition();
		}
		return m_cacheWorldMatrix.getPos();
	}

	Vector3 SceneComponent::getForwardVector() const
	{
		if (m_bTransformDirty)
		{
			return m_worldTransform.m_quat.rotate(Vector3(0.0f, 0.0f, 1.0f));
		}
		return m_cacheWorldMatrix.getN();
	}

	Vector3 SceneComponent::getRightVector() const
	{
		if (m_bTransformDirty)
		{
			return m_worldTransform.m_quat.rotate(Vector3(1.0f, 0.0f, 0.0f));
		}
		return m_cacheWorldMatrix.getU();
	}

	Vector3 SceneComponent::getUpVector() const
	{
		if (m_bTransformDirty)
		{
			return m_worldTransform.m_quat.rotate(Vector3(0.0f, 1.0f, 0.0f));
		}
		return m_cacheWorldMatrix.getV();
	}

	Matrix4x4 SceneComponent::getLocalTransform()
	{
		return m_localTransform;
	}

	Matrix4x4 SceneComponent::getWorldTransform()
	{
		return m_cacheWorldMatrix;
	}

}
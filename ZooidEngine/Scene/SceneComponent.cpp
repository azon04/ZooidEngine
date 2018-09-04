#include "SceneComponent.h"

#include "Events/Events.h"

namespace ZE {

	IMPLEMENT_CLASS_1(SceneComponent, Component);

	void SceneComponent::calculateTransform(const Matrix4x4& parentMat)
	{
		Matrix4x4::FastMul(parentMat, m_localTransform, m_worldTransform);
	}

	void SceneComponent::setupComponent()
	{
		Component::setupComponent();

		addEventDelegate(Event_UPDATE, &SceneComponent::handleUpdateEvent);
	}

	void SceneComponent::setLocalTransform(Matrix4x4& _localTransform)
	{
		m_localTransform = _localTransform;

		// #TODO Calculate new world transform to its children
	}

	void SceneComponent::setWorldTransform(Matrix4x4& _worldTransform)
	{
		m_worldTransform = _worldTransform;

		// #TODO Calculate new world transform to its children
	}

	void SceneComponent::setWorldPosition(const Vector3& _worldPosition)
	{
		m_worldTransform.setPos(_worldPosition);

		// #TODO Calculate new world transform to its children
	}

	void SceneComponent::handleUpdateEvent(Event* event)
	{
	}

	void SceneComponent::setRelativePosition(Vector3 _relativePosition)
	{
		m_localTransform.setPos(_relativePosition);

		// #TODO Calculate new world transform to its children
	}

	void SceneComponent::setScale(Vector3 _scale)
	{
		m_worldTransform.setScale(_scale);

		// #TODO Calculate new world transform to its children
	}

	void SceneComponent::rotateInDeg(Vector3 _eulerAngle)
	{
		m_worldTransform.rotateAroundU(DegToRad(_eulerAngle.getX()));
		m_worldTransform.rotateAroundV(DegToRad(_eulerAngle.getY()));
		m_worldTransform.rotateAroundN(DegToRad(_eulerAngle.getZ()));

		// #TODO Calculate new world transform to its children
	}

	Vector3 SceneComponent::getRelativePosition() const
	{
		return m_localTransform.getPos();
	}

	Vector3 SceneComponent::getWorldPosition() const
	{
		return m_worldTransform.getPos();
	}

	Vector3 SceneComponent::getForwardVector() const
	{
		return m_worldTransform.getN();
	}

	Vector3 SceneComponent::getRightVector() const
	{
		return m_worldTransform.getU();
	}

	Vector3 SceneComponent::getUpVector() const
	{
		return m_worldTransform.getV();
	}

	Matrix4x4 SceneComponent::getLocalTransform()
	{
		return m_localTransform;
	}

	Matrix4x4 SceneComponent::getWorldTransform()
	{
		return m_worldTransform;
	}

}
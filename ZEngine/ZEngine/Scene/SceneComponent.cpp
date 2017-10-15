#include "SceneComponent.h"

namespace ZE {

	IMPLEMENT_CLASS_1(SceneComponent, Component);

	void SceneComponent::calculateTransform(const Matrix4x4& parentMat)
	{
		m_worldTransform = m_transform * parentMat;
	}

}
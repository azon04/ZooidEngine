#include "PlayerTopCamera.h"

namespace ZE
{
	IMPLEMENT_CLASS_1(PlayerTopCamera, ZE::CameraComponent);

	void PlayerTopCamera::setupComponent()
	{
		ZE::CameraComponent::setupComponent();

		m_worldTransform.setPos(Vector3(60.0f, 0.0f, 150.0f));
	}


}


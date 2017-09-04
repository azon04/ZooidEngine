#ifndef __ZE_CAMERA_COMPONENT_H__
#define __ZE_CAMERA_COMPONENT_H__

#include "../GameObjectModel/Component.h"

namespace ZE
{
	class CameraComponent : public Component {
		DEFINE_CLASS(CameraComponent)

	public:
		CameraComponent(GameContext* gameContext) : Component(gameContext) {}
		virtual ~CameraComponent() {}

	};
}
#endif

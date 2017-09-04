#ifndef __ZE_CAMERA_MANAGER_H_
#define __ZE_CAMERA_MANAGER_H_

#include "../GameObjectModel/Component.h"

namespace ZE {
	
	class CameraManager : public Component {
		DEFINE_CLASS(CameraManager)

	public:
		CameraManager(GameContext* gameContext) : Component(gameContext) {}
		virtual ~CameraManager() {}
	};
}
#endif

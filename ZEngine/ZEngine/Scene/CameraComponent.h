#ifndef __ZE_CAMERA_COMPONENT_H__
#define __ZE_CAMERA_COMPONENT_H__

#include "SceneComponent.h"

namespace ZE
{
	class CameraComponent : public SceneComponent {
		DEFINE_CLASS(CameraComponent)

	public:
		CameraComponent(GameContext* gameContext) : SceneComponent(gameContext) {}
		virtual ~CameraComponent() {}


		void getViewMatrix(Matrix4x4& _outMat);
	};
}
#endif

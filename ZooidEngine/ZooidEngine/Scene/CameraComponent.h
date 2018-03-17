#ifndef __ZE_CAMERA_COMPONENT_H__
#define __ZE_CAMERA_COMPONENT_H__

#include "SceneComponent.h"

namespace ZE
{
	class CameraComponent : public SceneComponent {
		DEFINE_CLASS(CameraComponent)

	public:
		CameraComponent(GameContext* gameContext);
		virtual ~CameraComponent() {}

		void getViewMatrix(Matrix4x4& _outMat);

		ZE::Float32 m_near;
		ZE::Float32 m_far;
		ZE::Float32 m_degFieldOfView;

		// For Orthographic Projection
		ZE::Float32 m_orthoWidth;
		bool m_bUsingOrthoProjection;
	};
}
#endif

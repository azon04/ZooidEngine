#ifndef __ZE_CAMERA_COMPONENT_H__
#define __ZE_CAMERA_COMPONENT_H__

#include "SceneComponent.h"

namespace ZE
{
	class CameraComponent : public SceneComponent 
	{
		DEFINE_CLASS(CameraComponent)

	public:
		CameraComponent(GameContext* gameContext);
		virtual ~CameraComponent() {}

		// Get view matrix for camera
		void getViewMatrix(Matrix4x4& _outMat);
		void getProjectionMat(Matrix4x4& _outMat);
		void calculateProjectionMat();

		ZE::Float32 m_near;
		ZE::Float32 m_far;
		ZE::Float32 m_degFieldOfView;

		// For Orthographic Projection
		ZE::Float32 m_orthoWidth;
		bool m_bUsingOrthoProjection;

	private:

		Matrix4x4 m_projectionMat;
	};
}
#endif

#ifndef __ZE_VIEW_FUSTRUM_H__
#define __ZE_VIEW_FUSTRUM_H__

#include "Utils/PrimitiveTypes.h"
#include "Math/Vector3.h"
#include "Math/Plane.h"
#include "Math/Shapes.h"

#define USE_FUSTRUM_POINT 1

namespace ZE
{
	enum EFrustumTestResult : UInt8
	{
		FRUSTUM_OUTSIDE = 0,
		FRUSTUM_INSIDE,
		FRUSTUM_INTERSECT
	};

	enum EFrustumPoint : UInt8
	{
		FP_NTL = 0,
		FP_NTR,
		FP_NBR,
		FP_NBL,
		FP_FTL,
		FP_FTR,
		FP_FBR,
		FP_FBL
	};

	enum EFrustumPlane : UInt8
	{
		FPLANE_NEAR = 0,
		FPLANE_FAR,
		FPLANE_BOTTOM,
		FPLANE_TOP,
		FPLANE_RIGHT,
		FPLANE_LEFT
	};

	class Frustum
	{

	public:
		Frustum();
		~Frustum();

		void setMVPMatrix(Matrix4x4& mvpMatrix);

		EFrustumTestResult testPoint(Vector3& point);
		EFrustumTestResult testSphere(Sphere& sphere, Float32 _offset = 0.0f);
		EFrustumTestResult testAAB(AxisAlignedBox& aaBox, Float32 _offset = 0.0f);
		EFrustumTestResult testOB(OrientedBox& oBox, Float32 _offset = 0.0f);
		EFrustumTestResult testCylinder(Cylinder& cylinder);

	protected:
		Plane m_frustumPlanes[6];
	};

	class ViewFrustum : public Frustum
	{
	public:

		ViewFrustum();
		~ViewFrustum();

		void setProjectionVars(Float32 _ratio, Float32 _fovInDeg, Float32 _camNear, Float32 _camFar);
		void setCameraVars(Vector3& position, Vector3& direction, Vector3& up, Vector3& right);

		Vector3 getFrustumPoint(EFrustumPoint _point) const { return m_frustumPoints[_point]; }

	protected:

#if USE_FUSTRUM_POINT
		Vector3 m_nearCenter;
		Vector3 m_farCenter;
#endif

		Vector3 m_frustumPoints[8];
		
		Float32 m_ratio;
		Float32 m_fov;
		Float32 m_nearDist;
		Float32 m_farDist;
	};
}
#endif

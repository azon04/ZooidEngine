#ifndef __ZE_VIEW_FUSTRUM_H__
#define __ZE_VIEW_FUSTRUM_H__

#include "Utils/PrimitiveTypes.h"
#include "Math/Vector3.h"
#include "Math/Plane.h"
#include "Math/Shapes.h"

#define USE_FUSTRUM_POINT 0

namespace ZE
{
	enum EFustrumTestResult : UInt8
	{
		FUSTRUM_OUTSIDE = 0,
		FUSTRUM_INSIDE,
		FUSTRUM_INTERSECT
	};

	class ViewFustrum
	{
#if USE_FUSTRUM_POINT
		enum EFustrumPoint : UInt8
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
#endif
		enum EFustrumPlane : UInt8
		{
			FPLANE_NEAR = 0,
			FPLANE_FAR,
			FPLANE_BOTTOM,
			FPLANE_TOP,
			FPLANE_RIGHT,
			FPLANE_LEFT
		};

	public:

		ViewFustrum();
		~ViewFustrum();

		void setProjectionVars(Float32 _ratio, Float32 _fovInDeg, Float32 _camNear, Float32 _camFar);
		void setCameraVars(Vector3& position, Vector3& direction, Vector3& up, Vector3& right);

		EFustrumTestResult testPoint(Vector3& point);
		EFustrumTestResult testSphere(Sphere& sphere);
		EFustrumTestResult testAAB(AxisAlignedBox& aaBox);
		EFustrumTestResult testOB(OrientedBox& oBox);

	protected:

#if USE_FUSTRUM_POINT
		Vector3 m_fustrumPoints[8];
		Vector3 m_nearCenter;
		Vector3 m_farCenter;
#endif

		Plane m_fustrumPlane[6];
		
		Float32 m_ratio;
		Float32 m_fov;
		Float32 m_nearDist;
		Float32 m_farDist;
	};
}
#endif

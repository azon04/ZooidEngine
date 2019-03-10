#ifndef __ZE_VIEW_FUSTRUM_H__
#define __ZE_VIEW_FUSTRUM_H__

#include "Utils/PrimitiveTypes.h"
#include "Math/Vector3.h"
#include "Math/Plane.h"
#include "Math/Shapes.h"

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
		
		Vector3 m_fustrumPoints[8];
		Plane m_fustrumPlane[6];
		Vector3 m_nearCenter;
		Vector3 m_farCenter;

		Float32 m_ratio;
		Float32 m_fov;
		Float32 m_nearDist;
		Float32 m_farDist;
	};
}
#endif

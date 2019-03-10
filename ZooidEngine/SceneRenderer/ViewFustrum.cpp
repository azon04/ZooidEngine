#include "ViewFustrum.h"

namespace ZE
{

	ViewFustrum::ViewFustrum()
	{

	}

	ViewFustrum::~ViewFustrum()
	{

	}

	void ViewFustrum::setProjectionVars(Float32 _ratio, Float32 _fovInDeg, Float32 _camNear, Float32 _camFar)
	{
		m_ratio = _ratio;
		m_fov = DegToRad(_fovInDeg);
		m_nearDist = _camNear;
		m_farDist = _camFar;
	}

	void ViewFustrum::setCameraVars(Vector3& position, Vector3& direction, Vector3& up, Vector3& right)
	{
		m_nearCenter = position + direction * m_nearDist;
		m_farCenter = position + direction * m_farDist;

		Float32 wNear = 2 * tanf(m_fov * 0.5f) * m_nearDist;
		Float32 hNear = wNear * (1.0f / m_ratio);
		Float32 wFar = 2 * tanf(m_fov * 0.5f) * m_farDist;
		Float32 hFar = wFar * ( 1.0f / m_ratio );

		m_fustrumPoints[FP_NTL] = m_nearCenter + 0.5f * hNear * up + -0.5f * wNear * right;
		m_fustrumPoints[FP_NTR] = m_nearCenter + 0.5f * hNear * up + 0.5f * wNear * right;
		m_fustrumPoints[FP_NBR] = m_nearCenter + -0.5f * hNear * up + 0.5f * wNear * right;
		m_fustrumPoints[FP_NBL] = m_nearCenter + -0.5f * hNear * up + -0.5f * wNear * right;

		m_fustrumPoints[FP_FTL] = m_farCenter + 0.5f * hFar * up + -0.5f * wFar * right;
		m_fustrumPoints[FP_FTR] = m_farCenter + 0.5f * hFar * up + 0.5f * wFar * right;
		m_fustrumPoints[FP_FBR] = m_farCenter + -0.5f * hFar * up + 0.5f * wFar * right;
		m_fustrumPoints[FP_FBL] = m_farCenter + -0.5f * hFar * up + -0.5f * wFar * right;

		m_fustrumPlane[FPLANE_NEAR].fromNormalPosition(direction, m_nearCenter);
		m_fustrumPlane[FPLANE_FAR].fromNormalPosition(-1.0f * direction, m_farCenter);

		m_fustrumPlane[FPLANE_BOTTOM].fromPositions(m_fustrumPoints[FP_NBL], m_fustrumPoints[FP_NBR], m_fustrumPoints[FP_FBL]);
		m_fustrumPlane[FPLANE_TOP].fromPositions(m_fustrumPoints[FP_NTL], m_fustrumPoints[FP_FTL], m_fustrumPoints[FP_NTR]);

		m_fustrumPlane[FPLANE_LEFT].fromPositions(m_fustrumPoints[FP_FBL], m_fustrumPoints[FP_FTL], m_fustrumPoints[FP_NBL]);
		m_fustrumPlane[FPLANE_RIGHT].fromPositions(m_fustrumPoints[FP_FBR], m_fustrumPoints[FP_NBR], m_fustrumPoints[FP_FTR]);
	}

	ZE::EFustrumTestResult ViewFustrum::testPoint(Vector3& point)
	{
		EFustrumTestResult result = FUSTRUM_INSIDE;

		for (int i = 0; i < 6; i++)
		{
			if (m_fustrumPlane[i].distanceFromPlane(point) < 0)
			{
				return FUSTRUM_OUTSIDE;
			}
		}

		return result;
	}

	ZE::EFustrumTestResult ViewFustrum::testSphere(Sphere& sphere)
	{
		EFustrumTestResult result = FUSTRUM_INSIDE;

		for (int i = 0; i < 6; i++)
		{
			Float32 distance = m_fustrumPlane[i].distanceFromPlane(sphere.m_pos);
			if ( -distance > sphere.m_radius )
			{
				return FUSTRUM_OUTSIDE;
			}
			else if(distance < 0.0f)
			{
				result = FUSTRUM_INTERSECT;
			}
		}

		return result;
	}

	ZE::EFustrumTestResult ViewFustrum::testAAB(AxisAlignedBox& aaBox)
	{
		EFustrumTestResult result = FUSTRUM_INSIDE;

		for (int i = 0; i < 6; i++)
		{
			Vector3 posV = aaBox.m_min;
			Vector3 negV = aaBox.m_max;

			if (m_fustrumPlane[i].m_normal.getX() >= 0.0f)
			{
				posV.m_x = aaBox.m_max.m_x;
				negV.m_x = aaBox.m_min.m_x;
			}

			if (m_fustrumPlane[i].m_normal.getY() >= 0.0f)
			{
				posV.m_y = aaBox.m_max.m_y;
				negV.m_y = aaBox.m_min.m_y;
			}

			if (m_fustrumPlane[i].m_normal.getZ() >= 0.0f)
			{
				posV.m_z = aaBox.m_max.m_z;
				negV.m_z = aaBox.m_min.m_z;
			}

			if (m_fustrumPlane[i].distanceFromPlane(posV) < 0.0f)
			{
				return FUSTRUM_OUTSIDE;
			}
			else if (m_fustrumPlane[i].distanceFromPlane(negV) < 0.0f)
			{
				result = FUSTRUM_INTERSECT;
			}
		}

		return result;
	}

	ZE::EFustrumTestResult ViewFustrum::testOB(OrientedBox& oBox)
	{
		EFustrumTestResult result = FUSTRUM_INSIDE;

		Vector3 center = (oBox.m_max + oBox.m_min) * 0.5f;
		center = oBox.m_transform.mult(center);

		Vector3 halfBox = (oBox.m_max - oBox.m_min) * 0.5f;

		for (int i = 0; i < 6; i++)
		{
			float distance = m_fustrumPlane[i].distanceFromPlane(center);
			if (distance < 0.0f)
			{
				float r = abs(halfBox.getX() * m_fustrumPlane->m_normal.dotProduct(oBox.m_transform.getU())) +
					abs(halfBox.getY() * m_fustrumPlane->m_normal.dotProduct(oBox.m_transform.getV())) +
					abs(halfBox.getZ() * m_fustrumPlane->m_normal.dotProduct(oBox.m_transform.getN()));

				if (-distance > r)
				{
					return FUSTRUM_OUTSIDE;
				}
				else
				{
					result = FUSTRUM_INTERSECT;
				}
			}
		}

		return result;
	}

}
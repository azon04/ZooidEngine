#include "ViewFrustum.h"

namespace ZE
{

	ViewFrustum::ViewFrustum()
	{

	}

	ViewFrustum::~ViewFrustum()
	{

	}

	void ViewFrustum::setProjectionVars(Float32 _ratio, Float32 _fovInDeg, Float32 _camNear, Float32 _camFar)
	{
		m_ratio = _ratio;
		m_fov = DegToRad(_fovInDeg);
		m_nearDist = _camNear;
		m_farDist = _camFar;
	}

	void ViewFrustum::setCameraVars(Vector3& position, Vector3& direction, Vector3& up, Vector3& right)
	{
		Float32 wNear = 2 * tanf(m_fov * 0.5f) * m_nearDist;
		Float32 hNear = wNear * (1.0f / m_ratio);
		Float32 wFar = 2 * tanf(m_fov * 0.5f) * m_farDist;
		Float32 hFar = wFar * (1.0f / m_ratio);

#if !USE_FUSTRUM_POINT
		Vector3 nearCenter = position + direction * m_nearDist;
		Vector3 farCenter = position + direction * m_farDist;

		m_frustumPlanes[FPLANE_NEAR].fromNormalPosition(direction, nearCenter);
		m_frustumPlanes[FPLANE_FAR].fromNormalPosition(-1.0f * direction, farCenter);

		Vector3 leftBottomVector = (nearCenter + -0.5f * hNear * up + -0.5f * wNear * right) - position;
		Vector3 rightTopVector = (nearCenter + 0.5f * hNear * up + 0.5f * wNear * right) - position;
		Vector3 normalVector;

		normalVector = right.crossProduct(leftBottomVector);
		normalVector.normalize();
		m_frustumPlanes[FPLANE_BOTTOM].fromNormalPosition(normalVector, position);

		normalVector = rightTopVector.crossProduct(right);
		normalVector.normalize();
		m_frustumPlanes[FPLANE_TOP].fromNormalPosition(normalVector, position);

		normalVector = leftBottomVector.crossProduct(up);
		normalVector.normalize();
		m_frustumPlanes[FPLANE_LEFT].fromNormalPosition(normalVector, position);

		normalVector = up.crossProduct(rightTopVector);
		normalVector.normalize();
		m_frustumPlanes[FPLANE_RIGHT].fromNormalPosition(normalVector, position);

		m_frustumPoints[FP_NTL] = nearCenter + 0.5f * hNear * up + -0.5f * wNear * right;
		m_frustumPoints[FP_NTR] = nearCenter + 0.5f * hNear * up + 0.5f * wNear * right;
		m_frustumPoints[FP_NBR] = nearCenter + -0.5f * hNear * up + 0.5f * wNear * right;
		m_frustumPoints[FP_NBL] = nearCenter + -0.5f * hNear * up + -0.5f * wNear * right;

		m_frustumPoints[FP_FTL] = farCenter + 0.5f * hFar * up + -0.5f * wFar * right;
		m_frustumPoints[FP_FTR] = farCenter + 0.5f * hFar * up + 0.5f * wFar * right;
		m_frustumPoints[FP_FBR] = farCenter + -0.5f * hFar * up + 0.5f * wFar * right;
		m_frustumPoints[FP_FBL] = farCenter + -0.5f * hFar * up + -0.5f * wFar * right;
#else

		m_nearCenter = position + direction * m_nearDist;
		m_farCenter = position + direction * m_farDist;

		m_frustumPoints[FP_NTL] = m_nearCenter + 0.5f * hNear * up + -0.5f * wNear * right;
		m_frustumPoints[FP_NTR] = m_nearCenter + 0.5f * hNear * up + 0.5f * wNear * right;
		m_frustumPoints[FP_NBR] = m_nearCenter + -0.5f * hNear * up + 0.5f * wNear * right;
		m_frustumPoints[FP_NBL] = m_nearCenter + -0.5f * hNear * up + -0.5f * wNear * right;

		m_frustumPoints[FP_FTL] = m_farCenter + 0.5f * hFar * up + -0.5f * wFar * right;
		m_frustumPoints[FP_FTR] = m_farCenter + 0.5f * hFar * up + 0.5f * wFar * right;
		m_frustumPoints[FP_FBR] = m_farCenter + -0.5f * hFar * up + 0.5f * wFar * right;
		m_frustumPoints[FP_FBL] = m_farCenter + -0.5f * hFar * up + -0.5f * wFar * right;

		m_frustumPlanes[FPLANE_NEAR].fromNormalPosition(direction, m_nearCenter);
		m_frustumPlanes[FPLANE_FAR].fromNormalPosition(-1.0f * direction, m_farCenter);

		m_frustumPlanes[FPLANE_BOTTOM].fromPositions(m_frustumPoints[FP_NBL], m_frustumPoints[FP_NBR], m_frustumPoints[FP_FBL]);
		m_frustumPlanes[FPLANE_TOP].fromPositions(m_frustumPoints[FP_NTL], m_frustumPoints[FP_FTL], m_frustumPoints[FP_NTR]);

		m_frustumPlanes[FPLANE_LEFT].fromPositions(m_frustumPoints[FP_FBL], m_frustumPoints[FP_FTL], m_frustumPoints[FP_NBL]);
		m_frustumPlanes[FPLANE_RIGHT].fromPositions(m_frustumPoints[FP_FBR], m_frustumPoints[FP_NBR], m_frustumPoints[FP_FTR]);
#endif
	}

	ZE::EFrustumTestResult Frustum::testPoint(Vector3& point)
	{
		EFrustumTestResult result = FRUSTUM_INSIDE;

		for (int i = 0; i < 6; i++)
		{
			if (m_frustumPlanes[i].distanceFromPlane(point) < 0)
			{
				return FRUSTUM_OUTSIDE;
			}
		}

		return result;
	}

	ZE::EFrustumTestResult Frustum::testSphere(Sphere& sphere, Float32 _offset)
	{
		EFrustumTestResult result = FRUSTUM_INSIDE;

		for (int i = 0; i < 6; i++)
		{
			Float32 distance = m_frustumPlanes[i].distanceFromPlane(sphere.m_pos) + _offset;
			if ( -distance > sphere.m_radius )
			{
				return FRUSTUM_OUTSIDE;
			}
			else if(distance < 0.0f)
			{
				result = FRUSTUM_INTERSECT;
			}
		}

		return result;
	}

	ZE::EFrustumTestResult Frustum::testAAB(AxisAlignedBox& aaBox, Float32 _offset)
	{
		EFrustumTestResult result = FRUSTUM_INSIDE;

		for (int i = 0; i < 6; i++)
		{
			Vector3 posV = aaBox.m_min;
			Vector3 negV = aaBox.m_max;

			if (m_frustumPlanes[i].m_normal.getX() >= 0.0f)
			{
				posV.m_x = aaBox.m_max.m_x;
				negV.m_x = aaBox.m_min.m_x;
			}

			if (m_frustumPlanes[i].m_normal.getY() >= 0.0f)
			{
				posV.m_y = aaBox.m_max.m_y;
				negV.m_y = aaBox.m_min.m_y;
			}

			if (m_frustumPlanes[i].m_normal.getZ() >= 0.0f)
			{
				posV.m_z = aaBox.m_max.m_z;
				negV.m_z = aaBox.m_min.m_z;
			}

			if (m_frustumPlanes[i].distanceFromPlane(posV) + _offset < 0.0f)
			{
				return FRUSTUM_OUTSIDE;
			}
			else if (m_frustumPlanes[i].distanceFromPlane(negV) + _offset < 0.0f)
			{
				result = FRUSTUM_INTERSECT;
			}
		}

		return result;
	}

	ZE::EFrustumTestResult Frustum::testOB(OrientedBox& oBox, Float32 _offset)
	{
		EFrustumTestResult result = FRUSTUM_INSIDE;

		Vector3 u, v, n;
		u = oBox.m_transform.getU();
		v = oBox.m_transform.getV();
		n = oBox.m_transform.getN();

		Vector3 scaleVector = oBox.m_transform.extractScale();
		u = u / scaleVector.getX();
		v = v / scaleVector.getY();
		n = n / scaleVector.getZ();

		Vector3 center = (oBox.m_max + oBox.m_min) * 0.5f;
		center = oBox.m_transform.mult(center);

		Vector3 halfBox = (oBox.m_max - oBox.m_min) * 0.5f;
		halfBox = halfBox * scaleVector;

		for (int i = 0; i < 6; i++)
		{
			float distance = m_frustumPlanes[i].distanceFromPlane(center) + _offset;
			if (distance < 0.0f)
			{
				float r = abs(halfBox.getX() * m_frustumPlanes[i].m_normal.dotProduct(u)) +
					abs(halfBox.getY() * m_frustumPlanes[i].m_normal.dotProduct(v)) +
					abs(halfBox.getZ() * m_frustumPlanes[i].m_normal.dotProduct(n));

				if (-distance > r)
				{
					return FRUSTUM_OUTSIDE;
				}
				else
				{
					result = FRUSTUM_INTERSECT;
				}
			}
		}

		return result;
	}

	ZE::EFrustumTestResult Frustum::testCylinder(Cylinder& cylinder)
	{
		EFrustumTestResult result = FRUSTUM_INSIDE;
		Vector3 p = cylinder.m_p2 - cylinder.m_p2;
		p.normalize();

		for (int i = 0; i > 6; i++)
		{
			// Calculate effective radius
			float effectiveRadius = cylinder.m_radius * sqrt(1 - m_frustumPlanes[i].m_normal.dotProduct(p));

			// test distance to P1
			float distanceP1 = m_frustumPlanes[i].distanceFromPlane(cylinder.m_p1);
			if (distanceP1 < 0.0f)
			{
				// test distance to P2
				float distanceP2 = m_frustumPlanes[i].distanceFromPlane(cylinder.m_p2);
				if (distanceP2 < -effectiveRadius)
				{
					return FRUSTUM_OUTSIDE;
				}
				else
				{
					result = FRUSTUM_INTERSECT;
				}
			}
			else if (distanceP1 < effectiveRadius)
			{
				result = FRUSTUM_INTERSECT;
			}
			else // P1 is inside check if P2 is outside
			{
				// test distance to P2
				float distanceP2 = m_frustumPlanes[i].distanceFromPlane(cylinder.m_p2);
				if (distanceP2 < effectiveRadius)
				{
					result = FRUSTUM_INTERSECT;
				}
			}
		}

		return result;
	}

	Frustum::Frustum()
	{

	}

	Frustum::~Frustum()
	{

	}

	void Frustum::setMVPMatrix(Matrix4x4& mvpMatrix)
	{
		// Left plane
		m_frustumPlanes[FPLANE_LEFT].m_normal.m_x = mvpMatrix.m_data[1][4] + mvpMatrix.m_data[1][1];
		m_frustumPlanes[FPLANE_LEFT].m_normal.m_y = mvpMatrix.m_data[2][4] + mvpMatrix.m_data[2][1];
		m_frustumPlanes[FPLANE_LEFT].m_normal.m_z = mvpMatrix.m_data[3][4] + mvpMatrix.m_data[3][1];
		m_frustumPlanes[FPLANE_LEFT].m_normal.normalize();
		m_frustumPlanes[FPLANE_LEFT].m_distance = mvpMatrix.m_data[4][4] + mvpMatrix.m_data[4][1];

		// Right plane
		m_frustumPlanes[FPLANE_RIGHT].m_normal.m_x = mvpMatrix.m_data[1][4] - mvpMatrix.m_data[1][1];
		m_frustumPlanes[FPLANE_RIGHT].m_normal.m_y = mvpMatrix.m_data[2][4] - mvpMatrix.m_data[2][1];
		m_frustumPlanes[FPLANE_RIGHT].m_normal.m_z = mvpMatrix.m_data[3][4] - mvpMatrix.m_data[3][1];
		m_frustumPlanes[FPLANE_RIGHT].m_normal.normalize();
		m_frustumPlanes[FPLANE_RIGHT].m_distance = mvpMatrix.m_data[4][4] - mvpMatrix.m_data[4][1];

		// Bottom plane
		m_frustumPlanes[FPLANE_BOTTOM].m_normal.m_x = mvpMatrix.m_data[1][4] + mvpMatrix.m_data[1][2];
		m_frustumPlanes[FPLANE_BOTTOM].m_normal.m_y = mvpMatrix.m_data[2][4] + mvpMatrix.m_data[2][2];
		m_frustumPlanes[FPLANE_BOTTOM].m_normal.m_z = mvpMatrix.m_data[3][4] + mvpMatrix.m_data[3][2];
		m_frustumPlanes[FPLANE_BOTTOM].m_normal.normalize();
		m_frustumPlanes[FPLANE_BOTTOM].m_distance = mvpMatrix.m_data[4][4] + mvpMatrix.m_data[4][2];

		// Top Plane
		m_frustumPlanes[FPLANE_TOP].m_normal.m_x = mvpMatrix.m_data[1][4] - mvpMatrix.m_data[1][2];
		m_frustumPlanes[FPLANE_TOP].m_normal.m_y = mvpMatrix.m_data[2][4] - mvpMatrix.m_data[2][2];
		m_frustumPlanes[FPLANE_TOP].m_normal.m_z = mvpMatrix.m_data[3][4] - mvpMatrix.m_data[3][2];
		m_frustumPlanes[FPLANE_TOP].m_normal.normalize();
		m_frustumPlanes[FPLANE_TOP].m_distance = mvpMatrix.m_data[4][4] - mvpMatrix.m_data[4][2];

		// Near Plane
		m_frustumPlanes[FPLANE_NEAR].m_normal.m_x = mvpMatrix.m_data[1][4] + mvpMatrix.m_data[1][3];
		m_frustumPlanes[FPLANE_NEAR].m_normal.m_y = mvpMatrix.m_data[2][4] + mvpMatrix.m_data[2][3];
		m_frustumPlanes[FPLANE_NEAR].m_normal.m_z = mvpMatrix.m_data[3][4] + mvpMatrix.m_data[3][3];
		m_frustumPlanes[FPLANE_NEAR].m_normal.normalize();
		m_frustumPlanes[FPLANE_NEAR].m_distance = mvpMatrix.m_data[4][4] + mvpMatrix.m_data[4][3];

		// Far Plane
		m_frustumPlanes[FPLANE_FAR].m_normal.m_x = mvpMatrix.m_data[1][4] - mvpMatrix.m_data[1][3];
		m_frustumPlanes[FPLANE_FAR].m_normal.m_y = mvpMatrix.m_data[2][4] - mvpMatrix.m_data[2][3];
		m_frustumPlanes[FPLANE_FAR].m_normal.m_z = mvpMatrix.m_data[3][4] - mvpMatrix.m_data[3][3];
		m_frustumPlanes[FPLANE_FAR].m_normal.normalize();
		m_frustumPlanes[FPLANE_FAR].m_distance = mvpMatrix.m_data[4][4] - mvpMatrix.m_data[4][3];

	}

}
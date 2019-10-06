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

		for (int i = 0; i < 6; i++)
		{
			// Calculate effective radius
			float cosTheta = m_frustumPlanes[i].m_normal.dotProduct(p);
			float effectiveRadius = cylinder.m_radius * sqrt(1.0f - cosTheta * cosTheta);

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

	ZE::EFrustumTestResult Frustum::testCone(Cone& cone)
	{
		EFrustumTestResult result = FRUSTUM_INSIDE;

		Vector3 p = cone.m_top - cone.m_bottom;
		p.normalize();

		for (int i = 0; i < 6; i++)
		{
			// Calculate effective radius
			float cosTheta = m_frustumPlanes[i].m_normal.dotProduct(p);
			float effectiveRadius = cone.m_radius * sqrt(1.0f - cosTheta * cosTheta);

			// test distance to bottom
			float distanceBottom = m_frustumPlanes[i].distanceFromPlane(cone.m_bottom);
			if (abs(distanceBottom) < effectiveRadius)
			{
				result = FRUSTUM_INTERSECT;
			}
			else if (distanceBottom < 0.0f)
			{
				// test top
				float distanceTop = m_frustumPlanes[i].distanceFromPlane(cone.m_top);
				if (distanceTop > 0.0f)
				{
					result = FRUSTUM_INTERSECT;
				}
				else
				{
					return FRUSTUM_OUTSIDE;
				}
			}
			else // Bottom is inside check if top is outside
			{
				// test top
				float distanceTop = m_frustumPlanes[i].distanceFromPlane(cone.m_top);
				if (distanceTop < 0.0f)
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

	void Frustum::constructFromMVPMatrix(Matrix4x4& mvpMatrix)
	{
		// Distance multipled by -1.0 because we have the depth view flpped. (#TODO This might be a problem in the future)
		// Left plane
		m_frustumPlanes[FPLANE_LEFT].m_normal.m_x = mvpMatrix.m_data[0][3] + mvpMatrix.m_data[0][0];
		m_frustumPlanes[FPLANE_LEFT].m_normal.m_y = mvpMatrix.m_data[1][3] + mvpMatrix.m_data[1][0];
		m_frustumPlanes[FPLANE_LEFT].m_normal.m_z = mvpMatrix.m_data[2][3] + mvpMatrix.m_data[2][0];
		m_frustumPlanes[FPLANE_LEFT].m_distance = (mvpMatrix.m_data[3][3] + mvpMatrix.m_data[3][0]) * -1.0f;
		m_frustumPlanes[FPLANE_LEFT].normalize();

		// Right plane
		m_frustumPlanes[FPLANE_RIGHT].m_normal.m_x = mvpMatrix.m_data[0][3] - mvpMatrix.m_data[0][0];
		m_frustumPlanes[FPLANE_RIGHT].m_normal.m_y = mvpMatrix.m_data[1][3] - mvpMatrix.m_data[1][0];
		m_frustumPlanes[FPLANE_RIGHT].m_normal.m_z = mvpMatrix.m_data[2][3] - mvpMatrix.m_data[2][0];
		m_frustumPlanes[FPLANE_RIGHT].m_distance = (mvpMatrix.m_data[3][3] - mvpMatrix.m_data[3][0]) * -1.0f;
		m_frustumPlanes[FPLANE_RIGHT].normalize();

		// Bottom plane
		m_frustumPlanes[FPLANE_BOTTOM].m_normal.m_x = mvpMatrix.m_data[0][3] + mvpMatrix.m_data[0][1];
		m_frustumPlanes[FPLANE_BOTTOM].m_normal.m_y = mvpMatrix.m_data[1][3] + mvpMatrix.m_data[1][1];
		m_frustumPlanes[FPLANE_BOTTOM].m_normal.m_z = mvpMatrix.m_data[2][3] + mvpMatrix.m_data[2][1];
		m_frustumPlanes[FPLANE_BOTTOM].m_distance = (mvpMatrix.m_data[3][3] + mvpMatrix.m_data[3][1]) * -1.0f;
		m_frustumPlanes[FPLANE_BOTTOM].normalize();

		// Top Plane
		m_frustumPlanes[FPLANE_TOP].m_normal.m_x = mvpMatrix.m_data[0][3] - mvpMatrix.m_data[0][1];
		m_frustumPlanes[FPLANE_TOP].m_normal.m_y = mvpMatrix.m_data[1][3] - mvpMatrix.m_data[1][1];
		m_frustumPlanes[FPLANE_TOP].m_normal.m_z = mvpMatrix.m_data[2][3] - mvpMatrix.m_data[2][1];
		m_frustumPlanes[FPLANE_TOP].m_distance = (mvpMatrix.m_data[3][3] - mvpMatrix.m_data[3][1]) * -1.0f;
		m_frustumPlanes[FPLANE_TOP].normalize();

		// Near Plane
		m_frustumPlanes[FPLANE_NEAR].m_normal.m_x = mvpMatrix.m_data[0][3] + mvpMatrix.m_data[0][2];
		m_frustumPlanes[FPLANE_NEAR].m_normal.m_y = mvpMatrix.m_data[1][3] + mvpMatrix.m_data[1][2];
		m_frustumPlanes[FPLANE_NEAR].m_normal.m_z = mvpMatrix.m_data[2][3] + mvpMatrix.m_data[2][2];
		m_frustumPlanes[FPLANE_NEAR].m_distance = (mvpMatrix.m_data[3][3] + mvpMatrix.m_data[3][2]) * -1.0f;
		m_frustumPlanes[FPLANE_NEAR].normalize();

		// Far Plane
		m_frustumPlanes[FPLANE_FAR].m_normal.m_x = mvpMatrix.m_data[0][3] - mvpMatrix.m_data[0][2];
		m_frustumPlanes[FPLANE_FAR].m_normal.m_y = mvpMatrix.m_data[1][3] - mvpMatrix.m_data[1][2];
		m_frustumPlanes[FPLANE_FAR].m_normal.m_z = mvpMatrix.m_data[2][3] - mvpMatrix.m_data[2][2];
		m_frustumPlanes[FPLANE_FAR].m_distance = (mvpMatrix.m_data[3][3] - mvpMatrix.m_data[3][2]) * -1.0f;
		m_frustumPlanes[FPLANE_FAR].normalize();

	}

}
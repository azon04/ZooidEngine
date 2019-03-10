#ifndef __PLANE_Z__
#define __PLANE_Z__

#include "MathUtil.h"
#include "Utils/Macros.h"

class Plane
{
public:

	Plane() :
		m_distance(0)
	{}

	Plane(Vector3& normal, Vector3& pos)
	{
		fromNormalPosition(normal, pos);
	}

	Plane(Vector3& normal, ZE::Float32 dist)
	{
		fromNormalDistance(normal, dist);
	}

	~Plane() {}

	void fromNormalPosition(Vector3& normal, Vector3& point)
	{
		m_normal = normal;
		m_distance = normal.dotProduct(point);
	}

	void fromNormalDistance(Vector3& normal, ZE::Float32 dist)
	{
		m_normal = normal;
		m_distance = dist;
	}

	// Normal Calculation: (pos2 - pos1) * (pos3 - pos1)
	void fromPositions(Vector3& pos1, Vector3& pos2, Vector3& pos3)
	{
		m_normal = (pos2 - pos1).crossProduct(pos3 - pos1);
		m_normal.normalize();
		m_distance = m_normal.dotProduct(pos1);
	}

	ZE::Float32 distanceFromPlane(Vector3& point)
	{
		return m_normal.dotProduct(point) - m_distance;
	}

public:
	Vector3 m_normal;
	ZE::Float32 m_distance; // Distance plane from origin
};

#endif

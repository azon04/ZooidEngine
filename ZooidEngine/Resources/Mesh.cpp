#include "Mesh.h"

#include "Utils/Array.h"

namespace ZE
{
	IMPLEMENT_CLASS_0(Mesh)

	#define MATH_MAX(a,b) (a > b ? (a) : (b))

	Sphere Mesh::getBoundingSphere(const Matrix4x4& transform)
	{
		Sphere sphere;
		Vector3 scale = transform.extractScale();
		sphere.m_pos = transform.getPos() + (scale * m_centerOffset);
		Float32 maxScale = MATH_MAX(scale.m_x, MATH_MAX(scale.m_y, scale.m_z));
		sphere.m_radius = maxScale * m_radius;
		return sphere;
	}

	Sphere Mesh::getBoundingSphere(const Vector3& scale, const Vector3& position)
	{
		Sphere sphere;
		sphere.m_pos = position + (scale * m_centerOffset);
		Float32 maxScale = MATH_MAX(scale.m_x, MATH_MAX(scale.m_y, scale.m_z));
		sphere.m_radius = maxScale * m_radius;
		return sphere;
	}

	Sphere Mesh::getBoundingSphere(const Transform& transform)
	{
		Sphere sphere;
		Vector3 scale = transform.getScale();
		sphere.m_pos = transform.getPosition() + (scale * m_centerOffset);
		Float32 maxScale = MATH_MAX(scale.m_x, MATH_MAX(scale.m_y, scale.m_z));
		sphere.m_radius = maxScale * m_radius;
		return sphere;
	}

	AxisAlignedBox Mesh::getAABBoundingBox(const Matrix4x4& transform)
	{
		AxisAlignedBox aab;
		aab.m_min = Vector3(999.0f);
		aab.m_max = Vector3(-999.0f);

		Vector3 vertices[2] = { m_boxMin, m_boxMax };
		for (int i = 0; i < 2; i++)
		{
			for (int j = 0; j < 2; j++)
			{
				for (int k = 0; k < 2; k++)
				{
					Vector3 vertex(vertices[i].m_x, vertices[j].m_y, vertices[k].m_z);
					vertex = transform.mult(vertex);

					if (aab.m_min.m_x > vertex.m_x)
					{
						aab.m_min.m_x = vertex.m_x;
					}

					if (aab.m_min.m_y > vertex.m_y)
					{
						aab.m_min.m_y = vertex.m_y;
					}

					if (aab.m_min.m_z > vertex.m_z)
					{
						aab.m_min.m_z = vertex.m_z;
					}

					if (aab.m_max.m_x < vertex.m_x)
					{
						aab.m_max.m_x = vertex.m_x;
					}

					if (aab.m_max.m_y < vertex.m_y)
					{
						aab.m_max.m_y = vertex.m_y;
					}

					if (aab.m_max.m_z < vertex.m_z)
					{
						aab.m_max.m_z = vertex.m_z;
					}
				}
			}
		}
		return aab;
	}

	OrientedBox Mesh::getOBBoundingBox(const Matrix4x4& transform)
	{
		OrientedBox obb;
		obb.m_transform = transform;
		obb.m_min = m_boxMin;
		obb.m_max = m_boxMax;
		return obb;
	}

}
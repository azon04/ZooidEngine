#ifndef __TRANSFORM_Z__
#define __TRANSFORM_Z__

#include "Vector3.h"
#include "Matrix4x4.h"
#include "Quaternion.h"

class Transform
{
public:

	Transform() : m_quat(), m_position(0.0f), m_scale(1.0f) {}
	Transform(const Vector3& position, const Quaternion& quat, const Vector3& scale)
		: m_position(position), m_quat(quat), m_scale(scale)
	{}

	Transform(const Matrix4x4& matrix)
	{
		m_position = matrix.getPos();
		m_scale = matrix.extractScale();
		Matrix4x4 rotMat = matrix;
		rotMat.normalizeScale();
		m_quat = rotMat.toQuaternion();
	}

	FORCEINLINE Vector3 getPosition() const { return m_position; }
	FORCEINLINE Quaternion getQuat() const { return m_quat; }
	FORCEINLINE Vector3 getScale() const { return m_scale; }

	void setPosition(const Vector3& position)
	{
		m_position = position;
	}

	void setRotation(const Vector3& rotation)
	{
		// #TODO this is slow. Need to update to faster calculation
		m_quat = Quaternion(Vector3(1.0, 0.0, 0.0), rotation.getX())
			* Quaternion(Vector3(0.0, 1.0, 0.0), rotation.getY())
			* Quaternion(Vector3(0.0, 0.0, 1.0), rotation.getZ());
	}

	void setScale(const Vector3& scale)
	{
		m_scale = scale;
	}

	void setQuat(const Quaternion& quat)
	{
		m_quat = quat;
	}

	Matrix4x4 toMatrix() const
	{
		Matrix4x4 mat;
		mat.fromQuaternion(m_quat);
		mat.setScale(m_scale);
		mat.setPos(m_position);
		return mat;
	}

	Matrix4x4 toInverseMatrix() const
	{
		Matrix4x4 mat;
		mat.fromQuaternion(m_quat.conjugate());
		mat.setScale(1 / m_scale);
		mat.setPos(m_position*-1.0f);
		return mat;
	}

	Transform InvTransform()
	{
		return Transform(m_position*1.0f, m_quat.conjugate(), 1.0f / m_scale);
	}

public:

	Quaternion m_quat;
	Vector3 m_position;
	Vector3 m_scale;
};
#endif

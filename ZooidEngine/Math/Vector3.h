#ifndef __VECTOR_Z__
#define __VECTOR_Z__

#include "MathUtil.h"
#include "Utils/Macros.h"

class Vector3 {

public:
	Vector3() :
		m_x(0.0f), m_y(0.0f), m_z(0.0f)
	{}

	Vector3(float _x, float _y, float _z) :
		m_x(_x), m_y(_y), m_z(_z)
	{}

	Vector3(float _f) :
		Vector3(_f, _f, _f)
	{}

	Vector3(const Vector3& _v2) {
		m_x = _v2.m_x;
		m_y = _v2.m_y;
		m_z = _v2.m_z;
	}

	Vector3(float _data[3])
	{
		m_x = _data[0];
		m_y = _data[1];
		m_z = _data[2];
	}

	~Vector3() {}

	// Vector addition
	FORCEINLINE Vector3 operator+(const Vector3& _v2) const 
	{
		return Vector3(m_x + _v2.m_x, m_y + _v2.m_y, m_z + _v2.m_z);
	}

	// Vector subtraction
	FORCEINLINE Vector3 operator-(const Vector3& _v2) const 
	{
		return Vector3(m_x - _v2.m_x, m_y - _v2.m_y, m_z - _v2.m_z);
	}

	// Vector Division
	FORCEINLINE Vector3 operator/(const float _n) 
	{
		return Vector3(m_x / _n, m_y / _n, m_z / _n);
	}

	// Vector multiplication.
	FORCEINLINE Vector3 operator*(const Vector3& _v2) const 
	{
		return Vector3(m_x * _v2.m_x, m_y * _v2.m_y, m_z * _v2.m_z);
	}

	// Dot Product
	FORCEINLINE ZE::Float32 dotProduct(const Vector3& _v2) const
	{
		return m_x * _v2.m_x + m_y * _v2.m_y + m_z * _v2.m_z;
	}

	// Dot product
	ZE::Float32 operator|(const Vector3& _v2) const
	{
		return dotProduct(_v2);
	}

	// Cross Product
	FORCEINLINE Vector3 crossProduct(const Vector3& _v2) const
	{
		return Vector3(m_y * _v2.m_z - m_z * _v2.m_y,
			m_z * _v2.m_x - m_x * _v2.m_z,
			m_x * _v2.m_y - m_y * _v2.m_x);
	}

	// Cross product
	Vector3 operator^(const Vector3& _v2) const
	{
		return crossProduct(_v2);
	}

	// SquareLength
	FORCEINLINE ZE::Float32 lengthSquare() const
	{
		return m_x * m_x + m_y * m_y + m_z * m_z;
	}

	// Length
	FORCEINLINE ZE::Float32 length() const
	{
		return sqrt(m_x * m_x + m_y * m_y + m_z * m_z);
	}

	// Normalize
	void normalize() 
	{
		ZE::Float32 d = length();
		
		assert(d > 0.0f);

		m_x /= d;
		m_y /= d;
		m_z /= d;
	}

	// Getter and Setter
	FORCEINLINE ZE::Float32 getX() const { return m_x; }
	FORCEINLINE ZE::Float32 getY() const { return m_y; }
	FORCEINLINE ZE::Float32 getZ() const { return m_z; }

	FORCEINLINE Vector3& setX(float _x) { m_x = _x; return *this; }
	FORCEINLINE Vector3& setY(float _y) { m_y = _y; return *this; }
	FORCEINLINE Vector3& setZ(float _z) { m_z = _z; return *this; }

	// Helper function
	FORCEINLINE static Vector3 Normalize(const Vector3& v)
	{
		ZE::Float32 inverseLength = 1.0f / v.length();
		return Vector3(v.m_x * inverseLength, v.m_y * inverseLength, v.m_z * inverseLength);
	}

	// Data Member
	union 
	{
		struct {
			ZE::Float32 m_x, m_y, m_z;
		};

		ZE::Float32 m_data[3];
	};
	
};


// Vector multiplication
FORCEINLINE Vector3 operator*(const float _number, const Vector3& _v) 
{
	return Vector3(_v.m_x * _number, _v.m_y * _number, _v.m_z * _number);
}

FORCEINLINE Vector3 operator*(const Vector3& _v, const float _number) 
{
	return Vector3(_v.m_x * _number, _v.m_y * _number, _v.m_z * _number);
}

FORCEINLINE Vector3 operator/(const float number, const Vector3& v)
{
	return Vector3(number / v.m_x, number / v.m_y, number / v.m_z);
}

#endif

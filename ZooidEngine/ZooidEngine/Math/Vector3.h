#ifndef __VECTOR_Z__
#define __VECTOR_Z__

#include "MathUtil.h"
#include "Utils/Macros.h"
#include <cassert>

class Vector3 {

public:
	Vector3() :
		m_x(0.0f), m_y(0.0f), m_z(0.0f)
	{}

	Vector3(float _x, float _y, float _z) :
		m_x(_x), m_y(_y), m_z(_z)
	{}

	Vector3(const Vector3& _v2) {
		m_x = _v2.m_x;
		m_y = _v2.m_y;
		m_z = _v2.m_z;
	}

	~Vector3() {}

	// Vector Operation

	// Vector addition
	FORCEINLINE Vector3 operator+(const Vector3& _v2) {
		return Vector3(m_x + _v2.m_x, m_y + _v2.m_y, m_z + _v2.m_z);
	}

	// Vector subtraction
	FORCEINLINE Vector3 operator-(const Vector3& _v2) {
		return Vector3(m_x - _v2.m_x, m_y - _v2.m_y, m_z - _v2.m_z);
	}

	// Vector Division
	FORCEINLINE Vector3 operator/(const float _n) {
		return Vector3(m_x / _n, m_y / _n, m_z / _n);
	}

	// Vector multiplication.
	FORCEINLINE Vector3 operator*(const Vector3& _v2) {
		return Vector3(m_x * _v2.m_x, m_y * _v2.m_y, m_z * _v2.m_z);
	}

	// Dot Product
	FORCEINLINE ZE::Float32 dotProduct(const Vector3& _v2) {
		return m_x * _v2.m_x + m_y * _v2.m_y + m_z * _v2.m_z;
	}

	// Cross Product
	FORCEINLINE Vector3 crossProduct(const Vector3& _v2) {
		return Vector3(m_y * _v2.m_z - m_z * _v2.m_y,
			m_z * _v2.m_x - m_x * _v2.m_z,
			m_x * _v2.m_y - m_y * _v2.m_x);
	}

	// SquareLength
	FORCEINLINE ZE::Float32 lengthSquare() {
		return m_x * m_x + m_y * m_y + m_z * m_z;
	}

	// Length
	FORCEINLINE ZE::Float32 length() {
		return sqrt(m_x * m_x + m_y * m_y + m_z * m_z);
	}

	// Normalize
	void normalize() {
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

	// Data Member
	ZE::Float32 m_x, m_y, m_z;
};


// Vector multiplication
FORCEINLINE Vector3 operator*(const float _number, const Vector3& _v) {
	return Vector3(_v.m_x * _number, _v.m_y * _number, _v.m_z * _number);
}

FORCEINLINE Vector3 operator*(const Vector3& _v, const float _number) {
	return Vector3(_v.m_x * _number, _v.m_y * _number, _v.m_z * _number);
}


#endif

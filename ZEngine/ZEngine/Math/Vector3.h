#ifndef __VECTOR_Z__
#define __VECTOR_Z__

#include <cmath>

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
	Vector3 operator+(const Vector3& _v2) {
		return Vector3(m_x + _v2.m_x, m_y + _v2.m_y, m_z + _v2.m_z);
	}

	// Vector subtraction
	Vector3 operator-(const Vector3& _v2) {
		return Vector3(m_x - _v2.m_x, m_y - _v2.m_y, m_z - _v2.m_z);
	}

	// Vector Division
	Vector3 operator/(const float _n) {
		return Vector3(m_x / _n, m_y / _n, m_z / _n);
	}

	// Vector multiplication.
	Vector3 operator*(const Vector3& _v2) {
		return Vector3(m_x * _v2.m_x, m_y * _v2.m_y, m_z * _v2.m_z);
	}

	// Dot Product
	float dotProduct(const Vector3& _v2) {
		return m_x * _v2.m_x + m_y * _v2.m_y + m_z * _v2.m_z;
	}

	// Cross Product
	Vector3 crossProduct(const Vector3& _v2) {
		return Vector3(m_y * _v2.m_z - m_z * _v2.m_y,
			m_z * _v2.m_x - m_x * _v2.m_z,
			m_x * _v2.m_y - m_y * _v2.m_x);
	}

	// SquareLength
	float lengthSquare() {
		return m_x * m_x + m_y * m_y + m_z * m_z;
	}

	// Length
	float length() {
		return sqrt(m_x * m_x + m_y * m_y + m_z * m_z);
	}

	// Normalize
	float normalize() {
		float d = length();

		m_x /= d;
		m_y /= d;
		m_z /= d;
	}

	// Getter and Setter
	float getX() const { return m_x; }
	float getY() const { return m_y; }
	float getZ() const { return m_z; }

	Vector3& setX(float _x) { m_x = _x; return *this; }
	Vector3& setY(float _y) { m_y = _y; return *this; }
	Vector3& setZ(float _z) { m_z = _z; return *this; }

	// Data Member
	float m_x, m_y, m_z;
};


// Vector multiplication
inline Vector3 operator*(const float _number, const Vector3& _v) {
	return Vector3(_v.m_x * _number, _v.m_y * _number, _v.m_z * _number);
}

inline Vector3 operator*(const Vector3& _v, const float _number) {
	return Vector3(_v.m_x * _number, _v.m_y * _number, _v.m_z * _number);
}


#endif

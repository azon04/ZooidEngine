#ifndef __VECTOR2_Z__
#define __VECTOR2_Z__

#include "MathUtil.h"
#include <cassert>

class Vector2 {

public:

	Vector2() :
		m_x(0.0f), m_y(0.0f)
	{}

	Vector2(ZE::Float32 _x, ZE::Float32 _y) :
		m_x(_x), m_y(_y) 
	{}

	Vector2(const Vector2& _v2) {
		m_x = _v2.m_x;
		m_y = _v2.m_y;
	}

	~Vector2() {}

	// Vector Operation

	// Vector addition
	Vector2 operator+(const Vector2& _v2) {
		return Vector2(m_x + _v2.m_x, m_y + _v2.m_y);
	}


	// Vector substraction
	Vector2 operator-(const Vector2& _v2) {
		return Vector2(m_x - _v2.m_x, m_y - _v2.m_y);
	}

	// Vector division
	Vector2 operator/(const float& _scalar) {
		return Vector2(m_x / _scalar, m_y / _scalar);
	}

	// Vector multiplication
	Vector2 operator*(const Vector2& _v2) {
		return Vector2(m_x * _v2.m_x, m_y * _v2.m_y);
	}

	// Dot product
	ZE::Float32 dotProduct(Vector2 _v2) {
		return m_x * _v2.m_x + m_y * _v2.m_y;
	}

	// SquareLength
	ZE::Float32 lengthSquare() {
		return m_x * m_x + m_y + m_y;
	}


	// Length
	ZE::Float32 length() {
		return sqrt(m_x * m_x + m_y * m_y);
	}

	// Normalize
	void normalize() {
		ZE::Float32 d = length();

		ZASSERT(d > 0, "Length Zero");

		m_x /= d;
		m_y /= d;
	}

	// Getter and Setter
	ZE::Float32 getX() const { return m_x; }
	ZE::Float32 getY() const { return m_y; }

	Vector2& setX(float _x) { m_x = _x; return *this; }
	Vector2& setY(float _y) { m_y = _y; return *this; }
	
	ZE::Float32 m_x;
	ZE::Float32 m_y;
};

// Vector2 multiplication with scalar
inline Vector2 operator*(const float _number, const Vector2& _v) {
	return Vector2(_v.m_x * _number, _v.m_y * _number);
}

inline Vector2 operator*(const Vector2& _v, const float _number) {
	return Vector2(_v.m_x * _number, _v.m_y * _number);
}

#endif // __VECTOR2_Z__

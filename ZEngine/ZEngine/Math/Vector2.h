#ifndef __VECTOR2_Z__
#define __VECTOR2_Z__

#include <cmath>
#include <cassert>

class Vector2 {

public:

	Vector2() :
		m_x(0.0f), m_y(0.0f)
	{}

	Vector2(float _x, float _y) :
		m_x(_x), float m_y(_y) 
	{}

	Vector2(const Vector2& _v2) {
		m_x = _v2.m_x;
		m_y = _v2.m_y
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
	float dotProduct(Vector2 _v2) {
		return m_x * _v2.m_x + m_y * _v2.m_y;
	}

	// SquareLength
	float lengthSquare() {
		return m_x * m_x + m_y + m_y;
	}


	// Length
	float length() {
		return sqrt(m_x * m_x + m_y * m_y);
	}

	// Normalize
	void normalize() {
		float d = length();

		assert(d > 0);

		m_x /= d;
		m_y /= d;
	}

	// Getter and Setter
	float getX() const { return m_x; }
	float getY() const { return m_y; }

	Vector2& setX(float _x) { m_x = _x; return *this; }
	Vector2& setY(float _y) { m_y = _y; return *this; }
	
	float m_x;
	float m_y;
};

// Vector2 multiplication with scalar
inline Vector2 operator*(const float _number, const Vector2& _v) {
	return Vector2(_v.m_x * _number, _v.m_y * _number);
}

inline Vector2 operator*(const Vector2& _v, const float _number) {
	return Vector2(_v.m_x * _number, _v.m_y * _number);
}

#endif // __VECTOR2_Z__

#ifndef __QUATERNION_Z__
#define __QUATERNION_Z__

#include "Vector3.h"
#include "Matrix4x4.h"
#include "MathUtil.h"

class Quaternion {
	
public:

	Quaternion() 
		: m_x(0.0f), m_y(0.0f), m_z(0.0f), m_w(1.0f)
	{}

	Quaternion(float _x, float _y, float _z, float _w)
		: m_x(_x), m_y(_y), m_z(_z), m_w(_w)
	{}

	Quaternion(Vector3 _v, float _degreeAngle)
	{
		float _cos = cos(DegToRad(_degreeAngle) * 0.5f);
		float _sin = sin(DegToRad(_degreeAngle) * 0.5f);
		
		Vector3 _vSin = _v * _sin;
		m_x = _vSin.getX();
		m_y = _vSin.getY();
		m_z = _vSin.getZ();

		m_w = _cos;
	}

	Quaternion(const Quaternion& _q2) {
		m_x = _q2.m_x;
		m_y = _q2.m_y;
		m_z = _q2.m_z;
		m_w = _q2.m_w;
	}

	~Quaternion() {}
	
	// Methods
	void normalize() {
		float mag = sqrt(m_x * m_x + m_y * m_y + m_z * m_z + m_w * m_w);

		m_x /= mag;
		m_y /= mag;
		m_z /= mag;
		m_w /= mag;
	}

	float getAngleInDeg();
	float getAngleInRad();

	void updateAngleInRad(float _radAngle);
	void updateAngleInDeg(float _degAngle);

	// Operator Overloading
	Quaternion operator+(const Quaternion& _q2);
	Quaternion operator-(const Quaternion& _q2);
	Quaternion operator*(const Quaternion& _q2);

	// Matrix - Quaternion conversions
	Matrix4x4 toMatrix4x4();
	void fromMatrix4x4(const Matrix4x4& mat);

	// Getter and Setter
	float getX() const { return m_x; }
	float getY() const { return m_y; }
	float getZ() const { return m_z; }
	float getW() const { return m_w; }

	// DATA MEMBER
	float m_x;
	float m_y;
	float m_z;
	float m_w;
};

#endif // 

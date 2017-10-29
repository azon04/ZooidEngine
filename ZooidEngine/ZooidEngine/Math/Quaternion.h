#ifndef __QUATERNION_Z__
#define __QUATERNION_Z__

#include "Vector3.h"
#include "MathUtil.h"

class Quaternion {
	
public:

	Quaternion() 
		: m_x(0.0f), m_y(0.0f), m_z(0.0f), m_w(1.0f)
	{}

	Quaternion(ZE::Float32 _x, ZE::Float32 _y, ZE::Float32 _z, ZE::Float32 _w)
		: m_x(_x), m_y(_y), m_z(_z), m_w(_w)
	{}

	Quaternion(const Vector3& _v, ZE::Float32 _degreeAngle)
	{
		float _cos = cosf(DegToRad(_degreeAngle) * 0.5f);
		float _sin = sinf(DegToRad(_degreeAngle) * 0.5f);
		
		Vector3 _vSin = _v * _sin;
		m_x = _vSin.getX();
		m_y = _vSin.getY();
		m_z = _vSin.getZ();

		m_w = _cos;
	}

	Quaternion(const Vector3& _v)
		: m_x(_v.m_x), m_y(_v.m_y), m_z(_v.m_z), m_w(0)
	{}

	Quaternion(const Quaternion& _q2) {
		m_x = _q2.m_x;
		m_y = _q2.m_y;
		m_z = _q2.m_z;
		m_w = _q2.m_w;
	}

	~Quaternion() {}
	
	// Methods
	void normalize() {
		ZE::Float32 mag = sqrt(m_x * m_x + m_y * m_y + m_z * m_z + m_w * m_w);

		m_x /= mag;
		m_y /= mag;
		m_z /= mag;
		m_w /= mag;
	}

	ZE::Float32 getAngleInDeg() {
		return RadToDeg(getAngleInRad());
	}

	ZE::Float32 getAngleInRad() {
		return static_cast<float>(acos(m_w));
	}

	void updateAngleInRad(ZE::Float32 _radAngle) {
		float arccos = acos(m_w);
		float sinv = sin(arccos);
		
		float _cos = cos(_radAngle * 0.5f);
		float _sin = sin(_radAngle * 0.5f);

		m_w = _cos;

		m_x *= _sin / sinv;
		m_y *= _sin / sinv;
		m_z *= _sin / sinv;
	}

	void updateAngleInDeg(float _degAngle) {
		updateAngleInRad(DegToRad(_degAngle));
	}

	// Operator Overloading
	Quaternion operator+(const Quaternion& _q2) {
		return Quaternion(m_x + _q2.m_x, m_y + _q2.m_y, m_z + _q2.m_z, m_w + _q2.m_w);
	}

	Quaternion operator-(const Quaternion& _q2) {
		return Quaternion(m_x - _q2.m_x, m_y - _q2.m_y, m_z - _q2.m_z, m_w - _q2.m_w);
	}

	Quaternion operator*(const Quaternion& _q2) {
		float _w = m_w * _q2.m_w - (m_x * _q2.m_x + m_z * _q2.m_z + m_z * _q2.m_z);
		float _x = m_w * _q2.m_x + _q2.m_w * m_x + m_y * _q2.m_z - m_z * _q2.m_y;
		float _y = m_w * _q2.m_y + _q2.m_w * m_y - m_x * _q2.m_z + m_z * _q2.m_x;
		float _z = m_w * _q2.m_z + _q2.m_w * m_z - m_y * _q2.m_x + m_x * _q2.m_y;

		return Quaternion(_x, _y, _z, _w);
	}

	Vector3 rotateVector(const Vector3& _v, float _radAngle)
	{
		Quaternion p(*this), v(_v);
		p.updateAngleInRad(_radAngle);
		p.normalize();
		Quaternion pConj = *p;
		v = (p * v) * pConj;

		return v.toVector3();
	}

	Vector3 toVector3()
	{
		return Vector3(m_x, m_y, m_z);
	}
	// Quaternion Conjugate
	Quaternion operator*()
	{
		return Quaternion(-m_x, -m_y, -m_z, m_w);
	}

	// Getter and Setter
	ZE::Float32 getX() const { return m_x; }
	ZE::Float32 getY() const { return m_y; }
	ZE::Float32 getZ() const { return m_z; }
	ZE::Float32 getW() const { return m_w; }

	// DATA MEMBER
	ZE::Float32 m_x;
	ZE::Float32 m_y;
	ZE::Float32 m_z;
	ZE::Float32 m_w;
};

#endif // 

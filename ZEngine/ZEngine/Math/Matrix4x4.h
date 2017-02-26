#ifndef __MATRIX4X4_Z__
#define __MATRIX4X4_Z__

#include "Vector3.h"

class Matrix4x4 {
	
public:

	Matrix4x4() {
		m_data[0][0] = m_data[0][1] = m_data[0][2] = m_data[0][3] = 0.0f;
		m_data[1][0] = m_data[1][1] = m_data[1][2] = m_data[1][3] = 0.0f;
		m_data[2][0] = m_data[2][1] = m_data[2][2] = m_data[2][3] = 0.0f;
		m_data[3][0] = m_data[3][1] = m_data[3][2] = m_data[3][3] = 0.0f;
		m_data[0][0] = m_data[1][1] = m_data[2][2] = m_data[3][3] = 1.0f;
	}

	Matrix4x4(float _g11, float _g12, float _g13, float _g14,
		float _g21, float _g22, float _g23, float _g24,
		float _g31, float _g32, float _g33, float _g34,
		float _g41, float _g42, float _g43, float _g44) {
		m_data[0][0] = _g11; m_data[0][1] = _g12; m_data[0][2] = _g13; m_data[0][3] = _g14;
		m_data[0][1] = _g21; m_data[1][1] = _g22; m_data[1][2] = _g23; m_data[1][3] = _g24;
		m_data[0][2] = _g31; m_data[2][1] = _g32; m_data[2][2] = _g33; m_data[2][3] = _g34;
		m_data[0][3] = _g41; m_data[3][1] = _g42; m_data[3][2] = _g43; m_data[3][3] = _g44;
	}

	Matrix4x4(const Vector3& _u, const Vector3& _v, const Vector3& _n, const Vector3& _translation) {
		m_data[0][0] = m_data[0][1] = m_data[0][2] = m_data[0][3] = 0.0f;
		m_data[1][0] = m_data[1][1] = m_data[1][2] = m_data[1][3] = 0.0f;
		m_data[2][0] = m_data[2][1] = m_data[2][2] = m_data[2][3] = 0.0f;
		m_data[3][0] = m_data[3][1] = m_data[3][2] = m_data[3][3] = 0.0f;
		m_data[0][0] = m_data[1][1] = m_data[2][2] = m_data[3][3] = 1.0f;

		setU(_u);
		setV(_v);
		setN(_n);
		setPos(_translation);
	}

	Matrix4x4(const Matrix4x4& _m) {
		for (int r = 0; r < 4; r++) {
			for (int c = 0; c < 4; c++) {
				m_data[r][c] = _m.m_data[r][c];
			}
		}
	}

	~Matrix4x4() {}

	Matrix4x4 transpose() {
		Matrix4x4 newMat;
		for (int r = 0; r < 4; r++) {
			for (int c = 0; c < 4; c++) {
				newMat.m_data[r][c] = m_data[c][r];
			}
		}
		return newMat;
	}

	Matrix4x4 inverse();
	Matrix4x4 operator*(const Matrix4x4& _m);

	Vector3 mult(const Vector3 _v);
	Vector3 mult(const Vector3 _v, float _w);

	void scale(float _scale);
	void normalize();
	
	// Getter
	Vector3 getU();
	Vector3 getV();
	Vector3 getN();
	Vector3 getPos();

	// Setter
	void setU(const Vector3& _u);
	void setV(const Vector3& _v);
	void setN(const Vector3& _n);
	void setPos(const Vector3& _pos);

	// DATA MEMBER
	float m_data[4][4];
};

inline Vector3 operator*(const Vector3& _v, const Matrix4x4& _mat);

#endif // __MATRIX4X4_Z__

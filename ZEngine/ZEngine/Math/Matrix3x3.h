#ifndef __MATRIX3X3_Z__
#define __MATRIX3X3_Z__

#include "Vector3.h"

class Matrix3x3 {

public:

	Matrix3x3() {
		m_data[0][1] = m_data[0][2] = 0.0f;
		m_data[1][0] = m_data[1][2] = 0.0f;
		m_data[2][0] = m_data[2][1] = 0.0f;

		m_data[0][0] = m_data[1][1] = m_data[2][2] = 1.0f;
	}

	Matrix3x3(float _g11, float _g12, float _g13,
		float _g21, float _g22, float _g23,
		float _g31, float _g32, float _g33) {
		m_data[0][0] = _g11; m_data[0][1] = _g12; m_data[0][2] = _g13;
		m_data[0][0] = _g21; m_data[0][1] = _g22; m_data[0][2] = _g23;
		m_data[0][0] = _g31; m_data[0][1] = _g32; m_data[0][2] = _g33;
	}

	Matrix3x3(const Vector3& _u, const Vector3& _v, const Vector3& _n) {
		setU(_u);
		setV(_v);
		setN(_n);
	}

	Matrix3x3(const Matrix3x3& _m) {
		for (int r = 0; r < 3; r++) {
			for (int c = 0; c < 3; c++) {
				m_data[r][c] = _m.m_data[r][c];
			}
		}
	}

	~Matrix3x3() {}

	Matrix3x3 transpose() {
		Matrix3x3 newMat;
		for (int r = 0; r < 3; r++) {
			for (int c = 0; c < 3; c++) {
				newMat.m_data[r][c] = m_data[c][r];
			}
		}
		return newMat;
	}

	float det() {
		// #TODO Calculate Matrix3x3 Determinant
		return 0.0f;
	}

	Matrix3x3 inverse() {
		// #TODO implement Inverse for Matrix3x3

		return Matrix3x3();
	}

	Matrix3x3 operator*(const Matrix3x3& _m) {
		Matrix3x3 resultMat;

		for (int r = 0; r < 3; r++) {
			for (int c = 0; c < 3; c++) {
				resultMat.m_data[r][c] = 0.0f;
				for (int i = 0; i < 3; i++) {
					resultMat.m_data[r][c] += m_data[r][i] * _m.m_data[i][c];
				}
			}
		}

		return resultMat;
	}

	Vector3 mult(const Vector3& _v) {
		Vector3 resultVector;
		resultVector.m_x = _v.getX() * m_data[0][0] + _v.getY() * m_data[0][1] + _v.getZ() * m_data[0][2];
		resultVector.m_y = _v.getX() * m_data[1][0] + _v.getY() * m_data[1][1] + _v.getZ() * m_data[1][2];
		resultVector.m_z = _v.getX() * m_data[2][0] + _v.getY() * m_data[2][1] + _v.getZ() * m_data[2][2];
		
		return resultVector;
	}

	void scale(float _scale) {
		m_data[0][0] *= _scale;
		m_data[1][1] *= _scale;
		m_data[2][2] *= _scale;
	}

	void normalizeScale() {
		for (int r = 0; r < 3; r++) {
			float rowLength = sqrt(m_data[r][0] * m_data[r][0] + m_data[r][1] * m_data[r][1] + m_data[r][2] * m_data[r][2]);
			for (int c = 0; c < 3; c++) {
				m_data[r][c] /= rowLength;
			}
		}
	}

	// Getter
	Vector3 getU() {
		return Vector3(m_data[0][0], m_data[0][1], m_data[0][2]);
	}

	Vector3 getV() {
		return Vector3(m_data[1][0], m_data[1][1], m_data[1][2]);
	}

	Vector3 getN() {
		return Vector3(m_data[2][0], m_data[2][1], m_data[2][2]);
	}

	// Setter
	void setU(const Vector3& _u) {
		m_data[0][0] = _u.getX();
		m_data[0][1] = _u.getY();
		m_data[0][2] = _u.getZ();
	}

	void setV(const Vector3& _v) {
		m_data[1][0] = _v.getX();
		m_data[1][1] = _v.getY();
		m_data[1][2] = _v.getZ();
	}

	void setN(const Vector3& _n) {
		m_data[2][0] = _n.getX();
		m_data[2][1] = _n.getY();
		m_data[2][2] = _n.getZ();
	}

	// DATA MEMBER
	float m_data[3][3];
};

inline Vector3 operator*(const Vector3& _v, Matrix3x3& _mat) {
	return _mat.mult(_v);
};

#endif // 

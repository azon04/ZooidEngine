#ifndef __MATRIX3X3_Z__
#define __MATRIX3X3_Z__

#include "MathUtil.h"
#include "Vector3.h"

class Matrix3x3 {

public:

	Matrix3x3() {
		m_data[0][1] = m_data[0][2] = 0.0f;
		m_data[1][0] = m_data[1][2] = 0.0f;
		m_data[2][0] = m_data[2][1] = 0.0f;

		m_data[0][0] = m_data[1][1] = m_data[2][2] = 1.0f;
	}

	Matrix3x3(ZE::Float32 _g11, ZE::Float32 _g12, ZE::Float32 _g13,
		ZE::Float32 _g21, ZE::Float32 _g22, ZE::Float32 _g23,
		ZE::Float32 _g31, ZE::Float32 _g32, ZE::Float32 _g33) {
		m_data[0][0] = _g11; m_data[0][1] = _g12; m_data[0][2] = _g13;
		m_data[0][0] = _g21; m_data[0][1] = _g22; m_data[0][2] = _g23;
		m_data[0][0] = _g31; m_data[0][1] = _g32; m_data[0][2] = _g33;
	}

	// _values must be at least float[3][3]
	Matrix3x3(ZE::Float32 _values[3][3]) {
		for (int r = 0; r < 3; r++) {
			for (int c = 0; c < 3; c++) {
				m_data[r][c] = _values[r][c];
			}
		}
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

	ZE::Float32 det() {
		ZE::Float32 m11 = m_data[0][0] * (m_data[1][1] * m_data[2][2] - m_data[1][2] * m_data[2][1]);
		ZE::Float32 m12 = m_data[0][1] * (m_data[1][0] * m_data[2][2] - m_data[1][2] * m_data[2][0]);
		ZE::Float32 m13 = m_data[0][2] * (m_data[1][0] * m_data[2][1] - m_data[1][1] * m_data[2][0]);
		return m11 - m12 + m13;
	}

	ZE::Float32 matCofactor(ZE::Int32 row, ZE::Int32 col) {
		ZE::Float32 sign_one = ((row + col) % 2 == 0) ? 1.0f : -1.0f;
		ZE::Float32 m[2][2];
		int r_index = 0;
		int c_index = 0;
		
		for (int r = 0; r < 3; r++) {
			if(r == row) continue;
			for (int c = 0; c < 3; c++) {
				if(c == col) continue;
				m[r_index][c_index] = m_data[r][c];
				c_index++;
			}
			r_index++;
		}

		return sign_one * (m[0][0] * m[1][1] - m[0][1] * m[1][0]);
	}

	Matrix3x3 inverse() {
		Matrix3x3 inverseMatrix;
		ZE::Float32 d = det();

		for (int r = 0; r < 3; r++) {
			for (int c = 0; c < 3; c++) {
				inverseMatrix.m_data[r][c] = matCofactor(c, r) / d;
			}
		}

		return inverseMatrix;
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
	ZE::Float32 m_data[3][3];
};

inline Vector3 operator*(const Vector3& _v, Matrix3x3& _mat) {
	return _mat.mult(_v);
};

#endif // 

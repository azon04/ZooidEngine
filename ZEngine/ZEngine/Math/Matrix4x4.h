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

	float matMinor(int r, int c) {
		// #TODO Implement Minor, wait for 3x3 matrix
		return 0.0f;
	}

	float matCofactor(int r, int c) {
		int _sign = (r + c) % 2 == 0 ? 1 : -1;

		return _sign * matMinor(r, c);
	}

	float det() {
		
		float resultDet = 0.0f;
		
		for (int i = 0; i < 4; i++) {
			resultDet += m_data[0][i] * matCofactor(0, i);
		}

		return resultDet;
	}

	// fast inverse // use this if you're sure that the matrix is unitary scale, rotate on axis.
	Matrix4x4 inverse() {
		// #TODO Change this using Classical Adjoint
		// Implement Minors, Cofactors and Determinants
		// (RST) -1 = (T-1 S-1 R-1)
		
		Matrix4x4 R_inverse = *this;
		R_inverse.setPos(Vector3());
		R_inverse.normalizeScale();
		R_inverse = R_inverse.transpose();

		Matrix4x4 S_inverse = *this;
		S_inverse.setPos(Vector3());
		float length = getU().length();
		S_inverse.scale(1 / length);

		Matrix4x4 T_inverse;
		T_inverse.setPos(-1 * getPos());

		return T_inverse * S_inverse * R_inverse;
	}

	Matrix4x4 operator*(const Matrix4x4& _m) {
		Matrix4x4 resultMat;

		for (int r = 0; r < 4; r++) {
			for (int c = 0; c < 4; c++) {
				resultMat.m_data[r][c] = 0;
				for (int i = 0; i < 4; i++) {
					resultMat.m_data[r][c] += m_data[r][i] * _m.m_data[i][c];
				}
			}
		}

		return resultMat;
	}

	Vector3 mult(const Vector3 _v) {
		float w = 1.0f;
		Vector3 v = mult(_v, w);
		return v / w;
	}

	Vector3 mult(const Vector3 _v, float& _w) {
		Vector3 resultVector;
		resultVector.m_x = _v.getX() * m_data[0][0] + _v.getY() * m_data[1][0] + _v.getZ() * m_data[2][0] + _w * m_data[3][0];
		resultVector.m_y = _v.getX() * m_data[0][1] + _v.getY() * m_data[1][1] + _v.getZ() * m_data[2][1] + _w * m_data[3][1];
		resultVector.m_z = _v.getX() * m_data[0][2] + _v.getY() * m_data[1][2] + _v.getZ() * m_data[2][2] + _w * m_data[3][2];
		_w = _v.getX() * m_data[0][3] + _v.getY() * m_data[1][3] + _v.getZ() * m_data[2][3] + _w * m_data[3][3];

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

	Vector3 getPos() {
		return Vector3(m_data[3][0], m_data[3][1], m_data[3][2]);
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

	void setPos(const Vector3& _pos) {
		m_data[3][0] = _pos.getX();
		m_data[3][1] = _pos.getY();
		m_data[3][2] = _pos.getZ();
	}

	// DATA MEMBER
	float m_data[4][4];
};

inline Vector3 operator*(const Vector3& _v, Matrix4x4& _mat) {
	return _mat.mult(_v);
}

#endif // __MATRIX4X4_Z__

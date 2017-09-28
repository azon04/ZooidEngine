#ifndef __MATRIX4X4_Z__
#define __MATRIX4X4_Z__

#include "MathUtil.h"
#include "Vector3.h"
#include "Quaternion.h"
#include "Matrix3x3.h"

class Matrix4x4 {
	
public:

	Matrix4x4() {
		m_data[0][1] = m_data[0][2] = m_data[0][3] = 0.0f;
		m_data[1][0] = m_data[1][2] = m_data[1][3] = 0.0f;
		m_data[2][0] = m_data[2][1] = m_data[2][3] = 0.0f;
		m_data[3][0] = m_data[3][1] = m_data[3][2] = 0.0f;
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

	ZE::Float32 matMinor(ZE::Int32 row, ZE::Int32 column) {
		ZE::Float32 values[3][3];
		int r_index = 0;
		int c_index = 0;

		for (int r = 0; r < 3; r++) {
			if(r == row) continue;
			for (int c = 0; c < 3; c++) {
				if(c == column) continue;
				values[r_index][c_index] = m_data[r][c];
				c_index++;
			}
			r_index++;
		}

		Matrix3x3 mat((float**)values);
		return mat.det();
	}

	ZE::Float32 matCofactor(ZE::Int32 r, ZE::Int32 c) {
		int _sign = (r + c) % 2 == 0 ? 1 : -1;
		return _sign * matMinor(r, c);
	}

	ZE::Float32 det() {
		
		ZE::Float32 resultDet = 0.0f;
		
		for (int i = 0; i < 4; i++) {
			resultDet += m_data[0][i] * matCofactor(0, i);
		}

		return resultDet;
	}

	// fast inverse // use this if you're sure that the matrix is unitary scale, rotate on axis.
	Matrix4x4 inverse() {
		// Implement Minors, Cofactors and Determinants
		Matrix4x4 inverseMat;
		float d = det();

		for (int r = 0; r < 4; r++) {
			for (int c = 0; c < 4; c++) {
				inverseMat.m_data[r][c] = matCofactor(c, r) / d;
			}
		}

		return inverseMat;
#if 0
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
#endif
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

	void scale(ZE::Float32 _scale) {
		setU(getU() * _scale);
		setV(getV() * _scale);
		setN(getN() * _scale);
	}

	void scale(Vector3 _vScale)
	{
		setU(getU() * _vScale);
		setV(getV() * _vScale);
		setN(getN() * _vScale);
	}

	void normalizeScale() {
		for (int r = 0; r < 3; r++) {
			float rowLength = sqrt(m_data[r][0] * m_data[r][0] + m_data[r][1] * m_data[r][1] + m_data[r][2] * m_data[r][2]);
			for (int c = 0; c < 3; c++) {
				m_data[r][c] /= rowLength;
			}
		}
	}
	
	// Matrix - Quaternions
	void fromQuaternion(const Quaternion& _q) {
		float x = _q.getX();
		float y = _q.getY();
		float z = _q.getZ();
		float w = _q.getW();

		float xx = x*x;
		float yy = y*y;
		float zz = z*z;
		float xy = x*y;
		float xz = x*z;
		float xw = x*w;
		float yz = y*z;
		float yw = y*w;
		float zw = z*w;

		m_data[0][0] = 1 - 2 * (yy + zz);
		m_data[0][1] = 2 * (xy + zw);
		m_data[0][2] = 2 * (xz - yw);
		m_data[0][3] = 0.0f;

		m_data[1][0] = 2 * (xy - zw);
		m_data[1][1] = 1 - 2 * (xx + zz);
		m_data[1][2] = 2 * (xw + yz);
		m_data[1][3] = 0.0f;

		m_data[2][0] = 2 * (xz + yw);
		m_data[2][1] = 2 * (yz - xw);
		m_data[2][2] = 1 - 2 * (xx + yy);
		m_data[2][3] = 0.0f;

		m_data[3][0] = m_data[3][1] = m_data[3][2] = 0.0f;
		m_data[3][3] = 1.0f;
	}

	Quaternion toQuaternion() {
		float Trace = m_data[0][0] + m_data[1][1] + m_data[2][2] + m_data[3][3];

		Quaternion qResult;
		
		if (Trace >= 1.0f) {
			float _4w = 2 * sqrt(Trace);
			qResult.m_x = (m_data[1][2] - m_data[2][1]) / _4w;
			qResult.m_y = (m_data[2][0] - m_data[0][2]) / _4w;
			qResult.m_z = (m_data[0][1] - m_data[1][0]) / _4w;
			qResult.m_w = _4w / 4.0f;
		}
		else {
			float v[3];

			int i = 0, j, k;
			for (int d = 1; d < 3; d++) {
				if (m_data[d][d] > m_data[i][i]) {
					i = d;
				}
			}

			j = (i + 1) % 3;
			k = (k + 1) % 3;

			v[i] = sqrt(m_data[i][i] - m_data[j][j] - m_data[k][k] + 1) * 0.5f;
			
			float _4vi = 4 * v[i];
			
			v[j] = (m_data[i][j] + m_data[j][i]) / _4vi;
			v[k] = (m_data[i][k] + m_data[k][i]) / _4vi;

			qResult.m_w = (m_data[j][k] - m_data[k][j]) / _4vi;
			qResult.m_x = v[0];
			qResult.m_y = v[1];
			qResult.m_z = v[2];
		}

		return qResult;
	}

	// Translate Functions
	void translate(Vector3 _offset)
	{
		setPos(getPos() + _offset);
	}

	void moveForward(float _distance)
	{
		setPos(getPos() + getN() * _distance);
	}

	void moveUp(float _distance)
	{
		setPos(getPos() + getV() * _distance);
	}

	void moveSide(float _distance)
	{
		setPos(getPos() + getU() * _distance);
	}

	// Rotate Functions
	void rotateAroundU(float _radAngle)
	{
		Quaternion axis(getU());
		setV(axis.rotateVector(getV(), _radAngle));
		setN(axis.rotateVector(getN(), _radAngle));
	}

	void rotateAroundV(float _radAngle)
	{
		Quaternion axis(getV());
		setU(axis.rotateVector(getU(), _radAngle));
		setN(axis.rotateVector(getN(), _radAngle));
	}

	void rotateAroundN(float _radAngle)
	{
		Quaternion axis(getN());
		setU(axis.rotateVector(getU(), _radAngle));
		setV(axis.rotateVector(getV(), _radAngle));
	}

	void rotateAroundAxis(float _radAngle, Vector3 _v)
	{
		Quaternion axis(_v);
		setU(axis.rotateVector(getU(), _radAngle));
		setV(axis.rotateVector(getV(), _radAngle));
		setN(axis.rotateVector(getN(), _radAngle));
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
	ZE::Float32 m_data[4][4];
};

inline Vector3 operator*(const Vector3& _v, Matrix4x4& _mat) {
	return _mat.mult(_v);
};

#endif // __MATRIX4X4_Z__

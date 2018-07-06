#ifndef __MATRIX4X4_Z__
#define __MATRIX4X4_Z__

#include "MathUtil.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Quaternion.h"
#include "Matrix3x3.h"
#include "SSEHelper.h"

class Matrix4x4 {
	
public:

	Matrix4x4() 
	{
		m_data[0][1] = m_data[0][2] = m_data[0][3] = 0.0f;
		m_data[1][0] = m_data[1][2] = m_data[1][3] = 0.0f;
		m_data[2][0] = m_data[2][1] = m_data[2][3] = 0.0f;
		m_data[3][0] = m_data[3][1] = m_data[3][2] = 0.0f;
		m_data[0][0] = m_data[1][1] = m_data[2][2] = m_data[3][3] = 1.0f;
	}

	Matrix4x4(float _g11, float _g12, float _g13, float _g14,
		float _g21, float _g22, float _g23, float _g24,
		float _g31, float _g32, float _g33, float _g34,
		float _g41, float _g42, float _g43, float _g44) 
	{
		m_data[0][0] = _g11; m_data[0][1] = _g12; m_data[0][2] = _g13; m_data[0][3] = _g14;
		m_data[0][1] = _g21; m_data[1][1] = _g22; m_data[1][2] = _g23; m_data[1][3] = _g24;
		m_data[0][2] = _g31; m_data[2][1] = _g32; m_data[2][2] = _g33; m_data[2][3] = _g34;
		m_data[0][3] = _g41; m_data[3][1] = _g42; m_data[3][2] = _g43; m_data[3][3] = _g44;
	}

	Matrix4x4(const Vector3& _u, const Vector3& _v, const Vector3& _n, const Vector3& _translation) 
	{
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

	Matrix4x4(const Matrix4x4& _m) 
	{
		for (int r = 0; r < 4; r++) 
		{
			for (int c = 0; c < 4; c++) 
			{
				m_data[r][c] = _m.m_data[r][c];
			}
		}
	}

	~Matrix4x4() {}

	FORCEINLINE Matrix4x4 transpose() 
	{
		Matrix4x4 newMat;
		for (int r = 0; r < 4; r++) 
		{
			for (int c = 0; c < 4; c++) 
			{
				newMat.m_data[r][c] = m_data[c][r];
			}
		}
		return newMat;
	}

	// Minor of row,column of the matrix
	ZE::Float32 matMinor(ZE::Int32 row, ZE::Int32 column)
	{
		ZE::Float32 values[3][3];

		int r_index = 0;
		int c_index = 0;

		for (int r = 0; r < 4; r++) 
		{
			if(r == row) continue;
			c_index = 0;
			for (int c = 0; c < 4; c++) 
			{
				if(c == column) continue;
				values[r_index][c_index] = m_data[r][c];
				c_index++;
			}
			r_index++;
		}

		Matrix3x3 mat(values);
		return mat.det();
	}

	// Cofactor of r,c of the matrix
	ZE::Float32 matCofactor(ZE::Int32 r, ZE::Int32 c) 
	{
		int _sign = (r + c) % 2 == 0 ? 1 : -1;
		return _sign * matMinor(r, c);
	}

	FORCEINLINE ZE::Float32 det() 
	{
		ZE::Float32 resultDet = 0.0f;
		
		for (int i = 0; i < 4; i++) 
		{
			resultDet += m_data[0][i] * matCofactor(0, i);
		}

		return resultDet;
	}

	// fast inverse // use this if you're sure that the matrix is unitary scale, rotate on axis.
	Matrix4x4 inverse() 
	{
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

	Matrix4x4 operator*(const Matrix4x4& _m) 
	{
		Matrix4x4 resultMat;
		FastMul(*this, _m, resultMat);
		return resultMat;
	}

	FORCEINLINE Vector3 mult(const Vector3& _v) 
	{
		Vector3 resultVector;
		Matrix4x4::FastVecMul(_v, *this, resultVector);
		return resultVector;
	}

	void scale(ZE::Float32 _scale) 
	{
		setU(getU() * _scale);
		setV(getV() * _scale);
		setN(getN() * _scale);
	}

	void scale(Vector3 _vScale)
	{
		setU(getU() * _vScale.getX());
		setV(getV() * _vScale.getY());
		setN(getN() * _vScale.getZ());
	}

	Vector3 extractScale()
	{
		Vector3 scaleVector;
		scaleVector.setX(getU().length());
		scaleVector.setY(getV().length());
		scaleVector.setZ(getN().length());
		return scaleVector;
	}

	void normalizeScale() 
	{
		for (int r = 0; r < 3; r++) 
		{
			float rowLength = sqrt(m_data[r][0] * m_data[r][0] + m_data[r][1] * m_data[r][1] + m_data[r][2] * m_data[r][2]);
			for (int c = 0; c < 3; c++) 
			{
				m_data[r][c] /= rowLength;
			}
		}
	}
	
	// Matrix -> Quaternions conversion
	void fromQuaternion(const Quaternion& _q) 
	{
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

	Quaternion toQuaternion() 
	{
		float Trace = m_data[0][0] + m_data[1][1] + m_data[2][2] + m_data[3][3];

		Quaternion qResult;
		
		if (Trace >= 1.0f) 
		{
			float _4w = 2 * sqrt(Trace);
			qResult.m_x = (m_data[1][2] - m_data[2][1]) / _4w;
			qResult.m_y = (m_data[2][0] - m_data[0][2]) / _4w;
			qResult.m_z = (m_data[0][1] - m_data[1][0]) / _4w;
			qResult.m_w = _4w / 4.0f;
		}
		else 
		{
			float v[3];

			int i = 0;
			for (int d = 1; d < 3; d++) 
			{
				if (m_data[d][d] > m_data[i][i]) 
				{
					i = d;
				}
			}

			int j = (i + 1) % 3;
			int k = (k + 1) % 3;

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

	static FORCEINLINE void Mul(const Matrix4x4& m1, const Matrix4x4& m2, Matrix4x4& res)
	{
		for (int r = 0; r < 4; r++) 
		{
			for (int c = 0; c < 4; c++) 
			{
				res.m_data[r][c] = 0;
				for (int i = 0; i < 4; i++)
				{
					res.m_data[r][c] += m1.m_data[r][i] * m2.m_data[i][c];
				}
			}
		}
	}

	static FORCEINLINE void FastMul(const Matrix4x4& m1, const Matrix4x4& m2, Matrix4x4& res)
	{
#if USING_SSE
		mul2Matrices(m1.m_sseData, m2.m_sseData, res.m_sseData);
#else // Fallback if SSE not supported
		Mul(m1, m2, res);
#endif
	}

	static FORCEINLINE void VecMul(const Vector3& v, const Matrix4x4& m, Vector3& resultVector)
	{
		float _w = 1.0f;
		resultVector.m_x = v.getX() * m.m_data[0][0] + v.getY() * m.m_data[1][0] + v.getZ() * m.m_data[2][0] + _w * m.m_data[3][0];
		resultVector.m_y = v.getX() * m.m_data[0][1] + v.getY() * m.m_data[1][1] + v.getZ() * m.m_data[2][1] + _w * m.m_data[3][1];
		resultVector.m_z = v.getX() * m.m_data[0][2] + v.getY() * m.m_data[1][2] + v.getZ() * m.m_data[2][2] + _w * m.m_data[3][2];
		_w = v.getX() * m.m_data[0][3] + v.getY() * m.m_data[1][3] + v.getZ() * m.m_data[2][3] + _w * m.m_data[3][3];
		resultVector.m_x /= _w;
		resultVector.m_y /= _w;
		resultVector.m_z /= _w;
	}

	static FORCEINLINE void FastVecMul(const Vector3& _v, const Matrix4x4& m, Vector3& resultVector)
	{
#if USING_SSE
		__declspec(align(16)) float res[4];
		__m128 v;
		float _w = 1.0f;

		// Store in reverse because XMM using little endian (Intel is little endian)
		v = _mm_set_ps(_w, _v.m_z, _v.m_y, _v.m_x);

		v = mulVectorMatrix(v, m.m_sseData);

		_mm_store_ps(&res[0], v);
		resultVector.m_x = res[0] / res[3];
		resultVector.m_y = res[1] / res[3];
		resultVector.m_z = res[2] / res[3];
		_w = res[3];
#else // Fallback if SSE not supported
		VecMul(_v, m, resultVector);
#endif
	}

	static FORCEINLINE void Vec4Mul(const Vector4& v, const Matrix4x4& m, Vector4& resultVector)
	{
		resultVector.m_x = v.m_x * m.m_data[0][0] + v.m_y * m.m_data[1][0] + v.m_z * m.m_data[2][0] + v.m_w * m.m_data[3][0];
		resultVector.m_y = v.m_x * m.m_data[0][1] + v.m_y * m.m_data[1][1] + v.m_z * m.m_data[2][1] + v.m_w * m.m_data[3][1];
		resultVector.m_z = v.m_x * m.m_data[0][2] + v.m_y * m.m_data[1][2] + v.m_z * m.m_data[2][2] + v.m_w * m.m_data[3][2];
		resultVector.m_w = v.m_x * m.m_data[0][3] + v.m_y * m.m_data[1][3] + v.m_z * m.m_data[2][3] + v.m_w * m.m_data[3][3];
	}

	static FORCEINLINE void FastVec4Mul(const Vector4& v, const Matrix4x4& m, Vector4& resultVector)
	{
#if USING_SSE
		resultVector.m_sseData = mulVectorMatrix(v.m_sseData, m.m_sseData);
#else // Fall back
		Vec4Mul(v, m, resultVector);
#endif
	}

	FORCEINLINE Vector3 getU() const
	{
		return Vector3(m_data[0][0], m_data[0][1], m_data[0][2]);
	}

	FORCEINLINE Vector3 getV() const
	{
		return Vector3(m_data[1][0], m_data[1][1], m_data[1][2]);
	}

	FORCEINLINE Vector3 getN() const
	{
		return Vector3(m_data[2][0], m_data[2][1], m_data[2][2]);
	}

	FORCEINLINE Vector3 getPos() const
	{
		return Vector3(m_data[3][0], m_data[3][1], m_data[3][2]);
	}

	FORCEINLINE void setU(const Vector3& _u) 
	{
		m_data[0][0] = _u.getX();
		m_data[0][1] = _u.getY();
		m_data[0][2] = _u.getZ();
	}

	FORCEINLINE void setV(const Vector3& _v) 
	{
		m_data[1][0] = _v.getX();
		m_data[1][1] = _v.getY();
		m_data[1][2] = _v.getZ();
	}

	FORCEINLINE void setN(const Vector3& _n) 
	{
		m_data[2][0] = _n.getX();
		m_data[2][1] = _n.getY();
		m_data[2][2] = _n.getZ();
	}

	FORCEINLINE void setPos(const Vector3& _pos) 
	{
		m_data[3][0] = _pos.getX();
		m_data[3][1] = _pos.getY();
		m_data[3][2] = _pos.getZ();
	}

	// DATA MEMBER
	union 
	{
#if USING_SSE
		__m128 m_sseData[4];

		__declspec(align(16))
#endif
		ZE::Float32 m_data[4][4];
	};
};

FORCEINLINE Vector3 operator*(const Vector3& _v, Matrix4x4& _mat) 
{
	return _mat.mult(_v);
};

#endif // __MATRIX4X4_Z__

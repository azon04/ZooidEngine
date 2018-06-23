#include "MathOps.h"

namespace ZE 
{

	void MathOps::CreatePerspectiveProj(Matrix4x4& _result, Float32 _width, Float32 _height, Float32 _near, Float32 _far)
	{
		_result.m_data[0][0] = 2.0f * _near / _width;
		_result.m_data[1][0] = _result.m_data[2][0] = _result.m_data[3][0] = 0.0f;

		_result.m_data[1][1] = 2.0f * _near / _height;
		_result.m_data[0][1] = _result.m_data[2][1] = _result.m_data[3][1] = 0.0f;

		_result.m_data[2][2] = -(_far + _near) / (_far - _near);
		_result.m_data[3][2] = -(2.0f * _far * _near) / (_far - _near);
		_result.m_data[0][2] = _result.m_data[1][2] = 0.0f;

		_result.m_data[2][3] = -1;
		_result.m_data[0][3] = _result.m_data[1][3] = _result.m_data[3][3] = 0.0f;
	}

	void MathOps::CreatePerspectiveProjEx(Matrix4x4& _result, Float32 _aspectRatio, Float32 _degFOV, Float32 _near, Float32 _far)
	{
		Float32 cot = cosf(DegToRad(_degFOV) * 0.5f) / sinf(DegToRad(_degFOV) * 0.5f);

		_result.m_data[0][0] = cot;
		_result.m_data[1][0] = _result.m_data[2][0] = _result.m_data[3][0] = 0.0f;

		_result.m_data[1][1] = cot * _aspectRatio;
		_result.m_data[0][1] = _result.m_data[2][1] = _result.m_data[3][1] = 0.0f;

		_result.m_data[2][2] = -(_far + _near) / (_far - _near);
		_result.m_data[3][2] = -(2.0f * _far * _near) / (_far - _near);
		_result.m_data[0][2] = _result.m_data[2][1] = 0.0f;

		_result.m_data[2][3] = -1;
		_result.m_data[0][3] = _result.m_data[1][3] = _result.m_data[3][3] = 0.0f;
	}

	void MathOps::CreateOrthoProj(Matrix4x4& _result, Float32 _right, Float32 _top, Float32 _near, Float32 _far)
	{
		_result.m_data[0][0] = 1.0f / _right;
		_result.m_data[1][0] = _result.m_data[2][0] = _result.m_data[3][0] = 0.0f;

		_result.m_data[1][1] = 1.0f / _top;
		_result.m_data[0][1] = _result.m_data[2][1] = _result.m_data[3][1] = 0.0f;

		_result.m_data[2][2] = - 2 / (_far - _near);
		_result.m_data[3][2] = - (_far + _near) / (_far - _near);
		_result.m_data[0][2] = _result.m_data[2][1] = 0.0f;

		_result.m_data[3][3] = 1;
		_result.m_data[0][3] = _result.m_data[1][3] = _result.m_data[2][3] = 0.0f;
	}

	float MathOps::FLerp(float a, float b, float alpha)
	{
		return a + (b - a) * alpha;
	}

	Vector3 MathOps::VLerp(const Vector3& a, const Vector3& b, float alpha)
	{
		return a + (b - a) * alpha;
	}

	Quaternion MathOps::QSlerp(const Quaternion& q1, const Quaternion& q2, float alpha)
	{
		Float32 cosTheta = q1 | q2;

		Quaternion q1Temp = q1;

		if (cosTheta < 0.0f)
		{
			q1Temp = -1 * q1Temp;
			cosTheta = -cosTheta;
		}

		if (cosTheta > 0.995f)
		{
			Quaternion result = q1Temp + alpha * (q2 - q1Temp);
			result.normalize();
			return result;
		}

		Float32 theta_0 = acos(cosTheta);
		Float32 theta_1 = theta_0 * alpha;
		Float32 sin_theta_0 = sin(theta_0);
		Float32 sin_theta_1 = sin(theta_1);

		Float32 s0 = cos(theta_1) - cosTheta * sin_theta_1 / sin_theta_0;
		Float32 s1 = sin_theta_1 / sin_theta_0;

		return s0 * q1Temp + s1 * q2;

	}

	Quaternion MathOps::QLerp(const Quaternion& q1, const Quaternion& q2, float alpha)
	{
		return q1 + alpha * (q2 - q1);

	}

}
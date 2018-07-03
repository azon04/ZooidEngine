#ifndef __VECTOR4_Z__
#define __VECTOR4_Z__

#include "SSEHelper.h"
#include "MathUtil.h"
#include "Utils/Macros.h"
#include "Utils/PrimitiveTypes.h"
#include "Memory/MemoryHelper.h"

class Vector4 
{

public:
	Vector4() :
		m_x(0.0f), m_y(0.0), m_z(0.0f), m_w(0.0f)
	{}

	Vector4(float _x, float _y, float _z, float _w) :
		m_x(_x), m_y(_y), m_z(_z), m_w(_w)
	{}

	Vector4(const Vector4& _v)
	{
		ZE::MemoryHelper::Copy((void*)_v.m_data, m_data, sizeof(ZE::Float32) * 4);
	}

	~Vector4() {}

	// Vector Operations
	FORCEINLINE Vector4 operator+(const Vector4& _v2)
	{
		Vector4 res;
		Vector4::FastAdd(*this, _v2, res);
		return res;
	}

	FORCEINLINE Vector4 operator*(const Vector4& _v2)
	{
		Vector4 res;
		Vector4::FastMul(*this, _v2, res);
		return res;
	}

	FORCEINLINE ZE::Float32 lengthSquared()
	{
		return Vector4::FastDot(*this, *this);
	}

	FORCEINLINE ZE::Float32 length()
	{
		return sqrt(Vector4::FastDot(*this, *this));
	}

	FORCEINLINE static void Add(const Vector4& v1, const Vector4& v2, Vector4& res)
	{
		for (int i = 0; i < 4; i++)
		{
			res.m_data[i] = v1.m_data[i] + v2.m_data[i];
		}
	}

	FORCEINLINE static void FastAdd(const Vector4& v1, const Vector4& v2, Vector4& res)
	{
#if USING_SSE
		res.m_sseData = _mm_add_ps(v1.m_sseData, v2.m_sseData);
#else // Fallback
		Add(v1, v2, res);
#endif
	}

	FORCEINLINE static void Mul(const Vector4& v1, const Vector4& v2, Vector4& res)
	{
		for (int i = 0; i < 4; i++)
		{
			res.m_data[i] = v1.m_data[i] * v2.m_data[i];
		}
	}

	FORCEINLINE static void FastMul(const Vector4& v1, const Vector4& v2, Vector4& res)
	{
#if USING_SSE
		res.m_sseData = _mm_mul_ps(v1.m_sseData, v2.m_sseData);
#else // Fall back
		Mul(v1, v2, res);
#endif
	}

	FORCEINLINE static ZE::Float32 Dot(const Vector4& v1, const Vector4& v2)
	{
		return (v1.m_x * v2.m_x) + (v1.m_y * v2.m_y) + (v1.m_z * v2.m_z) + (v1.m_w * v2.m_w);
	}

	FORCEINLINE static ZE::Float32 FastDot(const Vector4& v1, const Vector4& v2)
	{
#if USING_SSE
		__m128 res;
		res = _mm_mul_ps(v1.m_sseData, v2.m_sseData);
		res = _mm_hadd_ps(res, res);
		res = _mm_hadd_ps(res, res);

		return res.m128_f32[0];
#else // Fall Back
		return Dot(v1, v2);
#endif
	}

	// Data Member
	union 
	{

#if USING_SSE
		__declspec(align(16))
#endif
		struct 
		{
			ZE::Float32 m_x, m_y, m_z, m_w;
		};

#if USING_SSE
		__declspec(align(16))
#endif
		ZE::Float32 m_data[4];

#if USING_SSE
		__m128 m_sseData;
#endif
	};
};

#endif

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

	FORCEINLINE void set(float _x, float _y, float _z, float _w)
	{
		m_x = _x; 
		m_y = _y; 
		m_z = _z; 
		m_w = _w;
	}

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

	FORCEINLINE ZE::Float32 lengthSquared() const
	{
		return Vector4::FastDot(*this, *this);
	}

	FORCEINLINE ZE::Float32 length() const
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
#if 0 // USING SSE / SSE2
		__m128 mul, shuf, sums;
		mul = _mm_mul_ps(v1.m_sseData, v2.m_sseData);
		
		// Calculates the sum of SSE Register - https://stackoverflow.com/a/35270026/195787
		shuf = _mm_movehdup_ps(mul);
		sums = _mm_add_ps(mul, shuf);
		shuf = _mm_movehl_ps(shuf, sums);
		sums = _mm_add_ps(sums, shuf);

		return _mm_cvtss_f32(sums);
#elif USING_AVX
		__m128 res = _mm_dp_ps(v1.m_sseData, v2.m_sseData, 0xff);

		return _mm_cvtss_f32(res);
#else // SSE3
		__m128 res = _mm_mul_ps(v1.m_sseData, v2.m_sseData);

		res = _mm_hadd_ps(res, res);
		res = _mm_hadd_ps(res, res);

		return _mm_cvtss_f32(res);
#endif
#else // Fall Back
		return Dot(v1, v2);
#endif
	}

	FORCEINLINE static void FastSqrt(const Vector4& v1, Vector4& res)
	{
#if USING_SSE
		res.m_sseData = _mm_sqrt_ps(v1.m_sseData);
#else // Fall Back
		res = Vector4(sqrt(v1.m_x), sqrt(v1.m_y), sqrt(v1.m_z), sqrt(v1.m_w));
#endif
	}

	// Getter
	FORCEINLINE ZE::Float32 getX() const { return m_x; }
	FORCEINLINE ZE::Float32 getY() const { return m_y; }
	FORCEINLINE ZE::Float32 getZ() const { return m_z; }
	FORCEINLINE ZE::Float32 getW() const { return m_w; }
	
	// Data Member
	union 
	{

		ALIGN16 struct 
		{
			ZE::Float32 m_x, m_y, m_z, m_w;
		};

		ALIGN16 ZE::Float32 m_data[4];

#if USING_SSE
		__m128 m_sseData;
#endif
	};
};

#endif

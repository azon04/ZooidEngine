// Consists all SSE function's implementations for the game engine
#ifndef __ZE_SSE_HELPER_H__
#define __ZE_SSE_HELPER_H__

#ifndef  USING_SSE
#define  USING_SSE 1
#endif

#if USING_SSE
#include "Utils/Macros.h"
#include <xmmintrin.h>

#define _mm_madd_ps(x, y, z) \
	_mm_add_ps(_mm_mul_ps((x),(y)),(z))

#define SHUFFLE_PARAM(x, y, z, w) \
	((x) | ((y) << 2) | ((z) << 4) | ((w) << 6))

#define _mm_replicate_x_ps(v) \
	_mm_shuffle_ps((v), (v), SHUFFLE_PARAM(0,0,0,0))

#define _mm_replicate_y_ps(v) \
	_mm_shuffle_ps((v), (v), SHUFFLE_PARAM(1,1,1,1))

#define _mm_replicate_z_ps(v) \
	_mm_shuffle_ps((v), (v), SHUFFLE_PARAM(2,2,2,2))

#define _mm_replicate_w_ps(v) \
	_mm_shuffle_ps((v), (v), SHUFFLE_PARAM(3,3,3,3))

FORCEINLINE __m128 mulVectorMatrix(const __m128& v, const __m128 rows[4])
{
	__m128 result = _mm_mul_ps(_mm_replicate_x_ps(v), rows[0]);
	result = _mm_madd_ps(_mm_replicate_y_ps(v), rows[1], result);
	result = _mm_madd_ps(_mm_replicate_z_ps(v), rows[2], result);
	result = _mm_madd_ps(_mm_replicate_w_ps(v), rows[3], result);
	return result;
}

FORCEINLINE __m128 mulVectorMatrixDivideByW(const __m128& v, const __m128 rows[4])
{
	__m128 result = _mm_mul_ps(_mm_replicate_x_ps(v), rows[0]);
	result = _mm_madd_ps(_mm_replicate_y_ps(v), rows[1], result);
	result = _mm_madd_ps(_mm_replicate_z_ps(v), rows[2], result);
	result = _mm_madd_ps(_mm_replicate_w_ps(v), rows[3], result);
	result = _mm_div_ps(result, _mm_replicate_w_ps(v));
	return result;
}

FORCEINLINE void mul2Matrices(const __m128 rowM1[4], const __m128 rowM2[4], __m128 result[4])
{
	result[0] = mulVectorMatrix(rowM1[0], rowM2);
	result[1] = mulVectorMatrix(rowM1[1], rowM2);
	result[2] = mulVectorMatrix(rowM1[2], rowM2);
	result[3] = mulVectorMatrix(rowM1[3], rowM2);
}

FORCEINLINE void mul3Matrices(const __m128 rowM1[4], const __m128 rowM2[4], const __m128 rowM3[4], __m128 result[4])
{
	mul2Matrices(rowM1, rowM2, result);
	mul2Matrices(result, rowM3, result);
}

FORCEINLINE __m128 add2Vector(const __m128 v1, const __m128 v2)
{
	return _mm_add_ps(v1, v2);
}

FORCEINLINE __m128 add3Vector(const __m128 v1, const __m128 v2, const __m128 v3)
{
	return add2Vector(add2Vector(v1, v2), v3);
}

#endif
#endif

#include "VectorTest.h"

#include "Utils/ZEngineHelper.h"

#include "Math/Vector3.h"
#include "Math/Vector4.h"

void VectorUnitTest::RunTest()
{
	Vector3 v1(1.0f, 0.0f, 0.0f);
	Vector3 v2(0.0f, 1.0f, 0.0f);
	Vector3 v3(0.0f, 0.0f, 1.0f);
	Vector3 v4(1.0f, 1.0f, 1.0f);
	Vector3 tmp;
	float f_tmp;

	// Vector addition
	tmp = v4 + v1;
	ZASSERT(tmp.getX() == 2.0f && tmp.getY() == 1.0f && tmp.getZ() == 1.0f, "Vector Addition Failed");
	
	// Vector subtraction
	tmp = v4 - v1;
	ZASSERT(tmp.getX() == 0.0f && tmp.getY() == 1.0f && tmp.getZ() == 1.0f, "Vector Subtraction Failed");

	// Vector multiplication
	tmp = v4 * 10;
	ZASSERT(tmp.getX() == 10.0f && tmp.getY() == 10.0f && tmp.getZ() == 10.0f, "Vector Multiplication Failed");

	// Vector division
	tmp = v4 / 2.0f;
	ZASSERT(tmp.getX() == 0.5f && tmp.getY() == 0.5f && tmp.getZ() == 0.5f, "Vector Division Failed");

	// Vector * Vector multiplication
	tmp = v4 * v2;
	ZASSERT(tmp.getX() == 0.0f && tmp.getY() == 1.0f && tmp.getZ() == 0.0f, "Vector-Vector Multiplication Failed");

	// Vector Dot Product
	f_tmp = v4.dotProduct(v1);
	ZASSERT(f_tmp == 1.0f, "Vector Dot Product Failed");

	// Vector Cross Product
	tmp = v1.crossProduct(v2);
	ZASSERT(tmp.getX() == 0.0f && tmp.getY() == 0.0f && tmp.getZ() == 1.0f, "Vector Dot Product Failed");

	// Vector Square Length
	f_tmp = v4.lengthSquare();
	ZASSERT(f_tmp == 3.0f, "Vector Dot Product Failed");

	// Vector Length
	f_tmp = v3.length();
	ZASSERT(f_tmp == 1.0f, "Vector Dot Product Failed");

	// Test Vector4
	Vector4 v4_1(1.0f, 2.0f, 3.0f, 4.0f);
	Vector4 v4_2(v4_1);
	Vector4 res;

	// Copy constructor
	ZASSERT((v4_2.m_x == v4_1.m_x) && (v4_2.m_y == v4_1.m_y) && (v4_2.m_z == v4_1.m_z) && (v4_2.m_w == v4_1.m_w), "Copy constructor Vector4 Failed");

	Vector4::FastAdd(v4_1, v4_2, res);
	ZASSERT(res.m_w == 8.0f, "Vector4::Add function Failed");

	Vector4::FastMul(v4_1, v4_2, res);
	ZASSERT(res.m_w == 16.0f, "Vector4::Mul function Failed");

	float dot = Vector4::FastDot(v4_1, v4_2);
	ZASSERT(dot == 30.0f, "Vector4::Dot function Failed");

}

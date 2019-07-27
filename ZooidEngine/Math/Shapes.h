#ifndef __SHAPES_Z__
#define __SHAPES_Z__

#include "Vector3.h"
#include "Matrix4x4.h"

class Shape { };

class AxisAlignedBox : public Shape
{
public:
	AxisAlignedBox() {}
	AxisAlignedBox(const Vector3& min, const Vector3& max) : m_min(min), m_max(max) {}

public:
	Vector3 m_min;
	Vector3 m_max;
};

class OrientedBox : public Shape
{
public:
	OrientedBox() {}

public:
	Matrix4x4 m_transform;

	Vector3 m_min;
	Vector3 m_max;
};

class Sphere : public Shape
{
public:
	Sphere() {}
	Sphere(const Vector3& pos, float radius) : m_pos(pos), m_radius(radius)
	{}

public:
	Vector3 m_pos;
	ZE::Float32 m_radius;
};

class Cylinder : public Shape
{
public:
	Cylinder() {}
	Cylinder(const Vector3& p1, const Vector3& p2, float radius)
		: m_p1(p1), m_p2(p2), m_radius(radius)
	{}

public:
	Vector3 m_p1, m_p2;
	ZE::Float32 m_radius;
};

class Cone : public Shape
{
public:
	Cone() {}
	Cone(const Vector3& top, const Vector3& bottom, float radius)
		: m_top(top), m_bottom(bottom), m_radius(radius) 
	{}

public:
	Vector3 m_top, m_bottom;
	ZE::Float32 m_radius;
};

#endif

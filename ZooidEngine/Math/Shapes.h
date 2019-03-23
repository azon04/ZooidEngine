#ifndef __SHAPES_Z__
#define __SHAPES_Z__

#include "Vector3.h"
#include "Matrix4x4.h"

class Shape { };

class AxisAlignedBox : public Shape
{
public:
	AxisAlignedBox() {}

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

public:
	Vector3 m_pos;
	ZE::Float32 m_radius;
};

class Cylinder : public Shape
{
public:
	Cylinder() {}

public:
	Vector3 m_p1, m_p2;
	ZE::Float32 m_radius;
};

#endif

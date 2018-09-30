#include "ShaderData.h"

#include "Memory/MemoryHelper.h"

namespace ZE
{

	void ShaderData::setViewMat(Matrix4x4& _viewMat4)
	{
		MemoryHelper::Copy(&_viewMat4.m_data[0][0], ViewMat, sizeof(Float32) * 16);
	}

	void ShaderData::setProjectionMat(Matrix4x4& _projectionMat4)
	{
		MemoryHelper::Copy(&_projectionMat4.m_data[0][0], ProjectionMat, sizeof(Float32) * 16);
	}

	void LightStruct::setPosition(Vector3& _position)
	{
		MemoryHelper::Copy(&_position.m_data[0], Position, sizeof(Float32) * 3);
	}

	void LightStruct::setDirection(Vector3& _direction)
	{
		MemoryHelper::Copy(&_direction.m_data[0], Direction, sizeof(Float32) * 3);
	}

	void LightStruct::setAmbient(Vector3& _ambient)
	{
		MemoryHelper::Copy(&_ambient.m_data[0], Ambient, sizeof(Float32) * 3);
	}

	void LightStruct::setDiffuse(Vector3& _diffuse)
	{
		MemoryHelper::Copy(&_diffuse.m_data[0], Diffuse, sizeof(Float32) * 3);
	}

	void LightStruct::setSpecular(Vector3& _specular)
	{
		MemoryHelper::Copy(&_specular.m_data[0], Specular, sizeof(Float32) * 3);
	}

	void LightStruct::setViewProjMatrix(Matrix4x4& _viewProj)
	{
		MemoryHelper::Copy(_viewProj.m_data, lightViewProj, sizeof(lightViewProj));
	}

	Vector3 LightStruct::getDirection()
	{
		return Vector3(Direction);
	}

	Vector3 LightStruct::getPosition()
	{
		return Vector3(Position);
	}

	void LightData::setViewPos(Vector3& _viewPos)
	{
		MemoryHelper::Copy(&_viewPos.m_data[0], ViewPos, sizeof(Float32) * 3);
	}

}
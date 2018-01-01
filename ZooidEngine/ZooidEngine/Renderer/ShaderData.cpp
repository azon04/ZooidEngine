#include "ShaderData.h"

#include "MemoryManagement/MemoryHelper.h"

namespace ZE
{

	void ShaderData::setViewMat(Matrix4x4& _viewMat4)
	{
		MemoryHelper::Copy(&_viewMat4.m_data[0][0], viewMat, sizeof(Float32) * 16);
	}

	void ShaderData::setProjectionMat(Matrix4x4& _projectionMat4)
	{
		MemoryHelper::Copy(&_projectionMat4.m_data[0][0], projectionMat, sizeof(Float32) * 16);
	}

	void LightStruct::setPosition(Vector3& _position)
	{
		MemoryHelper::Copy(&_position.m_data[0], position, sizeof(Float32) * 3);
	}

	void LightStruct::setDirection(Vector3& _direction)
	{
		MemoryHelper::Copy(&_direction.m_data[0], direction, sizeof(Float32) * 3);
	}

	void LightStruct::setAmbient(Vector3& _ambient)
	{
		MemoryHelper::Copy(&_ambient.m_data[0], ambient, sizeof(Float32) * 3);
	}

	void LightStruct::setDiffuse(Vector3& _diffuse)
	{
		MemoryHelper::Copy(&_diffuse.m_data[0], diffuse, sizeof(Float32) * 3);
	}

	void LightStruct::setSpecular(Vector3& _specular)
	{
		MemoryHelper::Copy(&_specular.m_data[0], specular, sizeof(Float32) * 3);
	}

	void LightData::setViewPos(Vector3& _viewPos)
	{
		MemoryHelper::Copy(&_viewPos.m_data[0], viewPos, sizeof(Float32) * 3);
	}

}
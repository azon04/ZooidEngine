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

}
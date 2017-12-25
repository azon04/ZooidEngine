#ifndef __ZE_SHADER_DATA_H__
#define __ZE_SHADER_DATA_H__

#include "Utils/PrimitiveTypes.h"
#include "Math/Matrix4x4.h"

namespace ZE
{
	struct ShaderData
	{
		Float32 viewMat[4][4];
		Float32 projectionMat[4][4];

		void setViewMat(Matrix4x4& _viewMat4);
		void setProjectionMat(Matrix4x4& _projectionMat4);
	};
}
#endif

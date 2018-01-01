#ifndef __ZE_SHADER_DATA_H__
#define __ZE_SHADER_DATA_H__

#include "Utils/PrimitiveTypes.h"
#include "Math/Matrix4x4.h"

#define MAX_NUMBER_OF_LIGHT 8

namespace ZE
{
	struct ShaderData
	{
		Float32 viewMat[4][4];
		Float32 projectionMat[4][4];

		void setViewMat(Matrix4x4& _viewMat4);
		void setProjectionMat(Matrix4x4& _projectionMat4);
	};

	struct LightStruct
	{
		Int32 type;

		// For flashlight
		Float32 cutOff;
		Float32 outerCutOff;

		// use for attenuation
		Float32 att_constant;
		Float32 att_linear;
		Float32 att_quadratic;

		// padding
		Float32 padding[2];

		Float32 position[4];
		Float32 direction[4];

		Float32 ambient[4];
		Float32 diffuse[4];
		Float32 specular[4];

		void setPosition(Vector3& _position);
		void setDirection(Vector3& _direction);
		void setAmbient(Vector3& _ambient);
		void setDiffuse(Vector3& _diffuse);
		void setSpecular(Vector3& _specular);

	};

	struct LightData
	{
		LightData() : numLight(0) { }

		Float32 viewPos[3];
		UInt32 numLight;

		LightStruct lights[MAX_NUMBER_OF_LIGHT];


		void setViewPos(Vector3& _viewPos);
	};
}
#endif

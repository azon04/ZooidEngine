#ifndef __MATH_OPS_Z__
#define __MATH_OPS_Z__

#include "Matrix4x4.h"
#include "Vector3.h"
#include "MathUtil.h"

namespace ZE
{
	namespace MathOps
	{
		
		void CreatePerspectiveProj(Matrix4x4& _result, Float32 _width, Float32 _height, Float32 _near, Float32 _far);

		void CreatePerspectiveProjEx(Matrix4x4& _result, Float32 _aspectRatio, Float32 _degFOV, Float32 _near, Float32 _far);
	
		void CreateOrthoProj(Matrix4x4& _result, Float32 _right, Float32 _top, Float32 _near, Float32 _far);
	}
}
#endif


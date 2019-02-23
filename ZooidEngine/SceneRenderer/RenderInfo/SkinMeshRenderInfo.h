#ifndef __ZE_SKINMESH_RENDERINFO_H__
#define __ZE_SKINMESH_RENDERINFO_H__

#include "MeshRenderInfo.h"

namespace ZE
{
	class IGPUBufferData;

	class SkinMeshRenderInfo : public MeshRenderInfo
	{

	public:
		IGPUBufferData* m_skinJointData;
	};
}
#endif

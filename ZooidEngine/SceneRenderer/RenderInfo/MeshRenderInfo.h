#ifndef __ZE_MESH_RENDER_INFO_H__
#define __ZE_MESH_RENDER_INFO_H__

#include "SceneRenderer/RenderInfo.h"
#include "Utils/PrimitiveTypes.h"

namespace ZE
{
	class Material;

	class MeshRenderInfo : public RenderInfo
	{
	public:
		MeshRenderInfo() :
			m_material(nullptr),
			m_castShadow(false),
			m_instanceCount(1)
		{}

		Material* m_material;
		UInt32 m_instanceCount;
		bool m_castShadow;
		bool m_isDoubleSided;
	};
};

#endif

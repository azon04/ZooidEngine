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
			m_instanceCount(1),
			m_outlined(false)
		{}

		Material* m_material;
		UInt32 m_instanceCount;
		bool m_castShadow:1;
		bool m_isDoubleSided:1;
		bool m_outlined:1;
	};
};

#endif

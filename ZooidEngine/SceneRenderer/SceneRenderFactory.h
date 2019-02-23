#ifndef __ZE_SCENE_RENDERFACTORY_H__
#define __ZE_SCENE_RENDERFACTORY_H__

namespace ZE
{
	class Mesh;
	class MeshRenderInfo;
	class SkinMeshRenderInfo;
	class RenderInfo;
	class IGPUBufferArray;

	class SceneRenderFactory
	{
	public:

		static MeshRenderInfo* CreateRenderInfoForMesh(Mesh* mesh);
		
		static void InitializeRenderInfoForMesh(MeshRenderInfo* renderInfo, Mesh* mesh);

		static RenderInfo* CreateRenderInfoForBufferArray(IGPUBufferArray* bufferArray);

	};
}
#endif

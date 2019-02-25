#ifndef __ZE_RENDER_GATHERER_H__
#define __ZE_RENDER_GATHERER_H__

#include "RenderInfo.h"
#include "RenderInfo/MeshRenderInfo.h"
#include "RenderInfo/SkinMeshRenderInfo.h"
#include "RenderInfo/TextRenderInfo.h"
#include "RenderInfo/SkyboxRenderInfo.h"

#include "Utils/Array.h"

namespace ZE
{
	template<class T>
	class RenderGatherer
	{
	public:
		RenderGatherer() {}
		virtual ~RenderGatherer() {}

		RenderInfo* getRenderInfos();

		T* nextRenderInfo();

		UInt32 getRenderCount();

		void reset();

	protected:
		Array<T> m_renderInfos;
	};

	template<class T>
	UInt32 ZE::RenderGatherer<T>::getRenderCount()
	{
		return m_renderInfos.size();
	}

	template<class T>
	RenderInfo* ZE::RenderGatherer<T>::getRenderInfos()
	{
		return reinterpret_cast<RenderInfo*> (m_renderInfos.data());
	}

	template<class T>
	T* ZE::RenderGatherer<T>::nextRenderInfo()
	{
		return new(m_renderInfos.allocateItem()) T;
	}

	template<class T>
	void ZE::RenderGatherer<T>::reset()
	{
		m_renderInfos.clear();
	}

	typedef RenderGatherer<RenderInfo> BaseRenderGatherer;
	typedef RenderGatherer<MeshRenderInfo> MeshRenderGatherer;
	typedef RenderGatherer<SkinMeshRenderInfo> SkinMeshRenderGatherer;
	typedef RenderGatherer<MeshRenderInfo> TransculentRenderGatherer;
	typedef RenderGatherer<TextRenderInfo> TextRenderGatherer;
	typedef RenderGatherer<SkyboxRenderInfo> SkyboxRenderGatherer;
}

#endif

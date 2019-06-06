#ifndef __ZE_SCENERENDERER_H__
#define __ZE_SCENERENDERER_H__

#include "Utils/PrimitiveTypes.h"
#include "Utils/Array.h"
#include "Common/SingletonClass.h"

namespace ZE
{
	class RenderInfo;
	class MeshRenderInfo;
	class IShaderChain;

	class ISceneRenderer
	{
	public:

		virtual void beginRender() = 0;

		virtual void render(RenderInfo* renderInfos, UInt32 renderInfoCount) = 0;

		virtual void endRender() = 0;

	};

	class BaseSceneRenderer : public ISceneRenderer
	{
	public:

		BaseSceneRenderer() : m_overrideShaderChain(nullptr) {}

		void setOverrideShaderChain(IShaderChain* overrideShaderChain) { m_overrideShaderChain = overrideShaderChain; }

	protected:
		IShaderChain* m_overrideShaderChain;
	};

	class MeshSceneRenderer : public BaseSceneRenderer, public Singleton<MeshSceneRenderer>
	{
	public:

		virtual void beginRender() override;
		virtual void render(RenderInfo* renderInfos, UInt32 renderInfoCount) override;
		virtual void endRender() override;

		static void Render(RenderInfo* renderInfos, UInt32 renderInfoCount, IShaderChain* shaderOverride = nullptr);
	};

	class TransculentSceneRenderer : public BaseSceneRenderer, public Singleton<TransculentSceneRenderer>
	{
	public:

		virtual void beginRender() override;
		virtual void render(RenderInfo* renderInfos, UInt32 renderInfoCount) override;
		virtual void endRender() override;

		static void Render(RenderInfo* renderInfos, UInt32 renderInfoCount);

	protected:
		void sortBlendInfos(MeshRenderInfo* renderInfos, UInt32 renderInfoCount, Array<UInt32>& result);
	};

	class SkinMeshSceneRenderer : public BaseSceneRenderer, public Singleton<SkinMeshSceneRenderer>
	{
	public:

		virtual void beginRender() override;
		virtual void render(RenderInfo* renderInfos, UInt32 renderInfoCount) override;
		virtual void endRender() override;

		static void Render(RenderInfo* renderInfos, UInt32 renderInfoCount, IShaderChain* shaderOverride = nullptr);

	};
};
#endif
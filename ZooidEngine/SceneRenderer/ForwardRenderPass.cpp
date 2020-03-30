#include "ForwardRenderPass.h"

#include "ShadowRenderer.h"
#include "SkyboxRenderer.h"
#include "TextRenderer.h"

#include "Resources/Texture.h"
#include "Renderer/DrawList.h"
#include "Renderer/IRenderer.h"
#include "Renderer/IGPUTexture.h"
#include "Renderer/IGPUFrameBuffer.h"
#include "Renderer/IGPURenderBuffer.h"
#include "Renderer/RenderZooid.h"
#include "ZEGameContext.h"
#include "Scene/CameraComponent.h"
#include "Scene/CameraManager.h"
#include "Renderer/IGPUBufferData.h"
#include "ZEGameContext.h"

namespace ZE
{
	void ForwardRenderPass::prepare(GameContext* _gameContext)
	{
		ScopedRenderThreadOwnership renderLock(_gameContext->getRenderer());

		if (!m_resultFrameBuffer)
		{
			TextureCreateDesc textureCreateDesc;
			textureCreateDesc.Width = _gameContext->getRenderer()->GetWidth();
			textureCreateDesc.Height = _gameContext->getRenderer()->GetHeight();
			textureCreateDesc.MinFilter = NEAREST;
			textureCreateDesc.MagFilter = NEAREST;
			textureCreateDesc.TextureFormat = TEX_RGBA16F;
			textureCreateDesc.DataType = FLOAT;
			textureCreateDesc.bGenerateMipMap = false;

			// Result Texture Buffer
			Handle textureHandle = _gameContext->getRenderZooid()->CreateRenderTexture();
			if (textureHandle.isValid())
			{
				m_resultTexture = textureHandle.getObject<IGPUTexture>();
				m_resultTexture->create(textureCreateDesc);
			}

			Handle depthRenderBuffer = _gameContext->getRenderZooid()->CreateRenderBuffer();
			if (depthRenderBuffer.isValid())
			{
				m_depthRenderBuffer = depthRenderBuffer.getObject<IGPURenderBuffer>();
				m_depthRenderBuffer->create(DEPTH_ONLY, textureCreateDesc.Width, textureCreateDesc.Height);
			}

			// Create Frame Buffer
			Handle fbHandle = _gameContext->getRenderZooid()->CreateFrameBuffer();
			if (fbHandle.isValid())
			{
				m_resultFrameBuffer = fbHandle.getObject<IGPUFrameBuffer>();
				m_resultFrameBuffer->bind();
				m_resultFrameBuffer->addTextureAttachment(COLOR_ATTACHMENT, m_resultTexture, 0);
				m_resultFrameBuffer->addRenderBufferAttachment(DEPTH_ATTACHMENT, m_depthRenderBuffer);
				m_resultFrameBuffer->setupAttachments();
				m_resultFrameBuffer->unbind();
			}
		}
	}

	void ForwardRenderPass::release(GameContext* _gameContext)
	{
		if (m_resultFrameBuffer) { m_resultFrameBuffer->release(); m_resultFrameBuffer = nullptr; }
		if (m_resultTexture) { m_resultTexture->release(); m_resultTexture = nullptr; }
		if (m_depthRenderBuffer) { m_depthRenderBuffer->release(); m_depthRenderBuffer = nullptr; }
	}

	void ForwardRenderPass::begin(GameContext* _gameContext)
	{
		RenderPass::begin(_gameContext);

		Matrix4x4 viewMat;

		ZE::CameraComponent* currentCamera = _gameContext->getCameraManager()->getCurrentCamera();
		if (currentCamera)
		{
			_gameContext->getDrawList()->m_shaderFrameData.setViewMat(_gameContext->getDrawList()->m_viewMat);
			_gameContext->getDrawList()->m_cameraPosition = currentCamera->getWorldPosition();
			_gameContext->getDrawList()->m_cameraDirection = currentCamera->getForwardVector();
			_gameContext->getDrawList()->m_shaderFrameData.setProjectionMat(_gameContext->getDrawList()->m_projectionMat);
			_gameContext->getDrawList()->m_lightData.setViewPos(currentCamera->getWorldPosition());
			_gameContext->getDrawList()->m_mainConstantBuffer->refresh();
		}
	}

	void ForwardRenderPass::end(GameContext* _gameContext)
	{
		RenderPass::end(_gameContext);
		addOutputTextureBuffer(m_resultTexture);
		addOutputFrameBuffer(m_resultFrameBuffer);
	}

	bool ForwardRenderPass::execute_CPU(GameContext* _gameContext)
	{
		return true;
	}

	bool ForwardRenderPass::execute_GPU(GameContext* _gameContext)
	{
		DrawList* drawList = _gameContext->getDrawList();

		ShadowDepthRenderer::Reset();

		// For each Light render Shadows
		for (UInt32 iShadowData = 0; iShadowData < drawList->m_lightShadowSize; iShadowData++)
		{
			LightShadowMapData& shadowMapData = drawList->m_lightShadowMapData[iShadowData];
			LightStruct& light = drawList->m_lightData.lights[shadowMapData.lightIndex];

			if (!shadowMapData.dynamicShadowFrameBuffer) { continue; }

			ShadowDepthRenderer::Setup(&shadowMapData, &light, drawList);
			ShadowDepthRenderer::BeginRender();
			
			ShadowDepthRenderer::Render(shadowMapData.meshRenderGatherer.getRenderInfos(), shadowMapData.meshRenderGatherer.getRenderCount(), false);
			ShadowDepthRenderer::Render(shadowMapData.skinMeshRenderGatherer.getRenderInfos(), shadowMapData.skinMeshRenderGatherer.getRenderCount(), true);
			ShadowDepthRenderer::EndRender();
		}

		ZCHECK(m_resultFrameBuffer);

		m_resultFrameBuffer->bind();

		_gameContext->getRenderer()->ClearScreen();

		// Refresh light constant buffer after generate the shadow maps
		_gameContext->getDrawList()->m_lightConstantBuffer->refresh();

		// reset viewport
		_gameContext->getRenderer()->ResetViewport();

		MeshSceneRenderer::Render(drawList->m_meshRenderGatherer.getRenderInfos(), drawList->m_meshRenderGatherer.getRenderCount());

		SkinMeshSceneRenderer::Render(drawList->m_skinMeshRenderGatherer.getRenderInfos(), drawList->m_skinMeshRenderGatherer.getRenderCount());

		TransculentSceneRenderer::Render(drawList->m_transculentRenderGatherer.getRenderInfos(), drawList->m_transculentRenderGatherer.getRenderCount());
		
		MeshSceneRenderer::Render(drawList->m_debugMeshRenderGatherer.getRenderInfos(), drawList->m_debugMeshRenderGatherer.getRenderCount());

		SkyBoxRenderer::Render(drawList->m_skyboxRenderGatherer.getRenderInfos(), drawList->m_skyboxRenderGatherer.getRenderCount());

		TextSceneRenderer::Render(drawList->m_textSceneRenderGatherer.getRenderInfos(), drawList->m_textSceneRenderGatherer.getRenderCount());
		TextScreenRenderer::Render(drawList->m_textScreenRenderGatherer.getRenderInfos(), drawList->m_textScreenRenderGatherer.getRenderCount());

		m_resultFrameBuffer->unbind();

		return true;
	}

}

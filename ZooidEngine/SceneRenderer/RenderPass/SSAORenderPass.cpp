#include "SSAORenderPass.h"

#include "Platform/Platform.h"
#include "Resources/Texture.h"
#include "Renderer/RenderZooid.h"
#include "Renderer/IGPUTexture.h"
#include "Renderer/IGPURenderBuffer.h"
#include "Renderer/IGPUFrameBuffer.h"
#include "Renderer/IRenderer.h"
#include "Renderer/IGPUStates.h"
#include "Renderer/DrawList.h"
#include "ResourceManagers/BufferManager.h"
#include "ResourceManagers/ShaderManager.h"
#include "Math/MathOps.h"

namespace ZE
{

	SSAORenderPass::SSAORenderPass()
		: m_shaderChain(nullptr),
		m_blurShaderChain(nullptr),
		m_noiseTexture(nullptr),
		m_ssaoResultTexture(nullptr),
		m_ssaoFrameBuffer(nullptr)
	{

	}

	void SSAORenderPass::prepare(GameContext* _gameContext)
	{
		ScopedRenderThreadOwnership renderLock(_gameContext->getRenderer());

		// Load Shader for this pass
		if (!m_shaderChain)
		{
			m_shaderChain = ShaderManager::GetInstance()->makeShaderChain("ZooidEngine/Shaders/DeferredShading/SSAOShader.vs", "ZooidEngine/Shaders/DeferredShading/SSAOShader.frag", nullptr, nullptr);
			m_blurShaderChain = ShaderManager::GetInstance()->makeShaderChain("ZooidEngine/Shaders/Utils/ScreenBlurShader.vs", "ZooidEngine/Shaders/Utils/ScreenBlurShader_1f.frag", nullptr, nullptr);
		}

		// Create Frame buffer and required textures
		if (!m_ssaoFrameBuffer)
		{
			// Generate SSAO kernel
			const int sampleCount = 32;
			m_ssaoKernels.reset(sampleCount);
			for (UInt32 i = 0; i < sampleCount; i++)
			{
				Vector3 sample(
					Platform::RandUnit() * 2.0f - 1.0f,
					Platform::RandUnit() * 2.0f - 1.0f,
					Platform::RandUnit()
				);
				sample.normalize();
				sample = sample * Platform::RandUnit();
				float scale = (float)i / 64.0f;
				scale = MathOps::FLerp(0.1f, 1.0f, scale * scale);
				sample = sample * scale;
				m_ssaoKernels.push_back(Vector4(sample.getX(), sample.getY(), sample.getZ(), 0.0f));
			}

			m_ssaoBufferData = BufferManager::getInstance()->createConstantBuffer(m_ssaoKernels.data(), sizeof(Vector4) * m_ssaoKernels.size(), 1, true);

			// Generate Noise Texture
			Array<Vector3> ssaoNoise;
			for (UInt32 i = 0; i < 16; i++)
			{
				Vector3 noise(Platform::RandUnit() * 2.0f - 1.0f,
					Platform::RandUnit() * 2.0f - 1.0f, 0.0f);
				ssaoNoise.push_back(noise);
			}

			TextureCreateDesc textureDesc;
			textureDesc.Width = 4;
			textureDesc.Height = 4;
			textureDesc.TextureFormat = TEX_RGB16F;
			textureDesc.MinFilter = NEAREST;
			textureDesc.MagFilter = NEAREST;
			textureDesc.DataType = FLOAT;
			textureDesc.bGenerateMipMap = false;
			textureDesc.InitialData = ssaoNoise.data();

			Handle noiseTextureHandle = _gameContext->getRenderZooid()->CreateRenderTexture();
			if (noiseTextureHandle.isValid())
			{
				m_noiseTexture = noiseTextureHandle.getObject<IGPUTexture>();
				m_noiseTexture->create(textureDesc);
			}

			textureDesc.Width = _gameContext->getRenderer()->GetWidth();
			textureDesc.Height = _gameContext->getRenderer()->GetHeight();
			textureDesc.TextureFormat = TEX_R16F;
			textureDesc.MinFilter = NEAREST;
			textureDesc.MagFilter = NEAREST;
			textureDesc.DataType = FLOAT;
			textureDesc.bGenerateMipMap = false;
			textureDesc.InitialData = nullptr;

			Handle ssaoTextureHandle = _gameContext->getRenderZooid()->CreateRenderTexture();
			if (ssaoTextureHandle.isValid())
			{
				m_ssaoResultTexture = ssaoTextureHandle.getObject<IGPUTexture>();
				m_ssaoResultTexture->create(textureDesc);
			}

			ssaoTextureHandle = _gameContext->getRenderZooid()->CreateRenderTexture();
			if (ssaoTextureHandle.isValid())
			{
				m_ssaoBlurResultTexture = ssaoTextureHandle.getObject<IGPUTexture>();
				m_ssaoBlurResultTexture->create(textureDesc);
			}

			Handle depthRenderBuffer = _gameContext->getRenderZooid()->CreateRenderBuffer();
			if (depthRenderBuffer.isValid())
			{
				m_depthRenderBuffer = depthRenderBuffer.getObject<IGPURenderBuffer>();
				m_depthRenderBuffer->create(DEPTH_ONLY, textureDesc.Width, textureDesc.Height);
			}

			// Create Frame Buffer
			Handle fbHandle = _gameContext->getRenderZooid()->CreateFrameBuffer();
			if (fbHandle.isValid())
			{
				m_ssaoFrameBuffer = fbHandle.getObject<IGPUFrameBuffer>();
				m_ssaoFrameBuffer->bind();
				m_ssaoFrameBuffer->addTextureAttachment(COLOR_ATTACHMENT, m_ssaoResultTexture, 0);
				m_ssaoFrameBuffer->addRenderBufferAttachment(DEPTH_ATTACHMENT, m_depthRenderBuffer);
				m_ssaoFrameBuffer->setupAttachments();
				m_ssaoFrameBuffer->unbind();
			}

			fbHandle = _gameContext->getRenderZooid()->CreateFrameBuffer();
			if (fbHandle.isValid())
			{
				m_ssaoBlurFrameBuffer = fbHandle.getObject<IGPUFrameBuffer>();
				m_ssaoBlurFrameBuffer->bind();
				m_ssaoBlurFrameBuffer->addTextureAttachment(COLOR_ATTACHMENT, m_ssaoBlurResultTexture, 0);
				m_ssaoBlurFrameBuffer->addRenderBufferAttachment(DEPTH_ATTACHMENT, m_depthRenderBuffer);
				m_ssaoBlurFrameBuffer->setupAttachments();
				m_ssaoBlurFrameBuffer->unbind();
			}
		}
	}

	void SSAORenderPass::release(GameContext* _gameContext)
	{
		if (m_ssaoFrameBuffer) { m_ssaoFrameBuffer->release(); m_ssaoFrameBuffer = nullptr; }
		if (m_depthRenderBuffer) { m_depthRenderBuffer->release(); m_depthRenderBuffer = nullptr; }
		if (m_ssaoResultTexture) { m_ssaoResultTexture->release(); m_ssaoResultTexture = nullptr; }
		if (m_noiseTexture) { m_noiseTexture->release(); m_noiseTexture = nullptr; }
	}

	void SSAORenderPass::begin(GameContext* _gameContext)
	{
		RenderPass::begin(_gameContext);
		
	}

	void SSAORenderPass::end(GameContext* _gameContext)
	{
		RenderPass::end(_gameContext);

		// Add to output
		addOutputTextureBuffer(m_ssaoBlurResultTexture);
	}

	bool SSAORenderPass::execute_CPU(GameContext* _gameContext)
	{
		return true;
	}

	bool SSAORenderPass::execute_GPU(GameContext* _gameContext)
	{
		IGPUBufferArray* quadVBO = BufferManager::getInstance()->getBufferArray(BUFFER_ARRAY_QUAD_V3_TC2);

		// Get Input
		ZASSERT(m_textureBufferInputs.size() >= 2, "Not enough input!");

		IGPUTexture* positionTexture = m_textureBufferInputs[0];
		IGPUTexture* normalTexture = m_textureBufferInputs[1];

		ZCHECK(m_ssaoFrameBuffer);
		m_ssaoFrameBuffer->bind();

		_gameContext->getRenderer()->ResetViewport();
		_gameContext->getRenderer()->ClearScreen();

		m_shaderChain->bind();
		
		m_shaderChain->setTexture("gPosition", positionTexture, 0);
		positionTexture->bind();

		m_shaderChain->setTexture("gNormal", normalTexture, 1);
		normalTexture->bind();

		m_shaderChain->setTexture("noiseTexture", m_noiseTexture, 2);
		m_noiseTexture->bind();

		m_shaderChain->bindConstantBuffer("frame_data", _gameContext->getDrawList()->m_mainConstantBuffer);
		_gameContext->getDrawList()->m_mainConstantBuffer->bind();

		m_shaderChain->bindConstantBuffer("sample_data", m_ssaoBufferData);
		m_ssaoBufferData->bind();

		_gameContext->getRenderer()->DrawBufferArray(TOPOLOGY_TRIANGLE, quadVBO, 6);

		m_shaderChain->unbind();

		m_ssaoFrameBuffer->unbind();

		ZCHECK(m_ssaoBlurFrameBuffer);
		m_ssaoBlurFrameBuffer->bind();

		_gameContext->getRenderer()->ClearScreen();

		m_blurShaderChain->bind();

		m_blurShaderChain->setTexture("inputTexture", m_ssaoResultTexture, 0);
		m_ssaoResultTexture->bind();

		_gameContext->getRenderer()->DrawBufferArray(TOPOLOGY_TRIANGLE, quadVBO, 6);

		m_blurShaderChain->unbind();

		m_ssaoBlurFrameBuffer->unbind();

		return true;
	}

}
#include "EMToIrradianceMap.h"
#include "Renderer/IRenderer.h"
#include "Renderer/RenderZooid.h"
#include "Renderer/IGPURenderBuffer.h"
#include "Renderer/IGPUFrameBuffer.h"
#include "Renderer/IGPUTexture.h"
#include "Renderer/IGPUStates.h"
#include "Renderer/BufferData.h"
#include "Resources/Texture.h"
#include "ResourceManagers/ShaderManager.h"
#include "ResourceManagers/BufferManager.h"
#include "Math/MathOps.h"
#include "ZEGameContext.h"
#include "SharedResources.h"

namespace ZE
{

	void EMToIrradianceMap::prepare(GameContext* _gameContext)
	{
		// Load Shader
		if (!m_shaderChain)
		{
			m_shaderChain = ShaderManager::GetInstance()->makeShaderChain("ZooidEngine/Shaders/Utils/EquiRectangularToCube.vs", "ZooidEngine/Shaders/Utils/EMToIrradianceMap.frag", nullptr, nullptr);
		}

		if (!m_frameBuffer)
		{
			// Create Depth Buffer
			Handle depthRenderBufferHandle = _gameContext->getRenderZooid()->CreateRenderBuffer();
			ZCHECK(depthRenderBufferHandle.isValid());
			m_depthBuffer = depthRenderBufferHandle.getObject<IGPURenderBuffer>();
			m_depthBuffer->create(DEPTH_ONLY, m_textureSize, m_textureSize);

			// Create the frame buffer
			Handle fbHandle = _gameContext->getRenderZooid()->CreateFrameBuffer();
			ZCHECK(fbHandle.isValid());

			m_frameBuffer = fbHandle.getObject<IGPUFrameBuffer>();

			m_frameBuffer->bind();
			m_frameBuffer->addRenderBufferAttachment(DEPTH_ATTACHMENT, m_depthBuffer);
			m_frameBuffer->unbind();
		}
	}

	void EMToIrradianceMap::release(GameContext* _gameContext)
	{
		if (m_shaderChain)
		{
			m_shaderChain->release();
			m_shaderChain = nullptr;
		}

		if (m_frameBuffer)
		{
			m_frameBuffer->release();
			m_frameBuffer = nullptr;

			m_depthBuffer->release();
			m_depthBuffer = nullptr;
		}
	}

	bool EMToIrradianceMap::execute_CPU(GameContext* _gameContext)
	{
		return true;
	}

	bool EMToIrradianceMap::execute_GPU(GameContext* _gameContext)
	{
		ZCHECK(m_environmentMap);

		IRenderer* renderer = _gameContext->getRenderer();

		// Create the cube texture result
		TextureCreateDesc cubeTextureDesc;
		cubeTextureDesc.Width = cubeTextureDesc.Height = m_textureSize;
		cubeTextureDesc.FaceCount = 6;
		cubeTextureDesc.WrapU = cubeTextureDesc.WrapV = CLAMP_TO_EDGE;
		cubeTextureDesc.MinFilter = LINEAR;
		cubeTextureDesc.DataType = FLOAT;
		cubeTextureDesc.bGenerateMipMap = false;
		cubeTextureDesc.TextureFormat = TEX_RGB16F;

		m_result = _gameContext->getRenderZooid()->CreateRenderTexture();
		if (!m_result.isValid())
		{
			return false;
		}

		IGPUTexture* CubeTexture = m_result.getObject<IGPUTexture>();
		CubeTexture->create(cubeTextureDesc);

		// Create the cube data array
		IGPUBufferArray* cubeBufferArray = UtilShaderResources::GetCubeVerticesOnly();
		ZCHECK(cubeBufferArray);

		// Create buffer data for shader
		Matrix4x4 vertexBufferData[2];
		BufferData bufferDataDraw(UNIFORM_BUFFER);
		bufferDataDraw.setStaticBuffer(false);
		bufferDataDraw.SetData(vertexBufferData[0].m_data, sizeof(Matrix4x4) * 2);

		IGPUBufferData* drawBufferData = BufferManager::getInstance()->createConstantBufferFromBuffer(&bufferDataDraw, 0);

		// Draw each view for cube map
		MathOps::CreatePerspectiveProjEx(vertexBufferData[0], 1.0f, 90.0f, 0.1f, 100.0f);
		Vector3 targets[] =
		{
			Vector3(1.0f, 0.0f, 0.0f),
			Vector3(-1.0f, 0.0f, 0.0f),
			Vector3(0.0f, 1.0f, 0.0f),
			Vector3(0.0f, -1.0f, 0.0f),
			Vector3(0.0f, 0.0f, 1.0f),
			Vector3(0.0f, 0.0f, -1.0f),
		};

		Vector3 ups[] =
		{
			Vector3(0.0f, -1.0f, 0.0f),
			Vector3(0.0f, -1.0f, 0.0f),
			Vector3(0.0f, 0.0f, 1.0f),
			Vector3(0.0f, 0.0f, -1.0f),
			Vector3(0.0f, -1.0f, 0.0f),
			Vector3(0.0f, -1.0f, 0.0f),
		};

		renderer->SetRenderRasterizerState(TRenderRasterizerState<EFaceFrontOrder::CCW, ECullFace::CULL_NONE, ERenderFillMode::MODE_FILL>::GetGPUState());
		renderer->SetViewport(0, 0, m_textureSize, m_textureSize);

		m_frameBuffer->bind();

		m_shaderChain->bind();

		drawBufferData->bind();
		m_shaderChain->bindConstantBuffer("draw_data", drawBufferData);

		m_environmentMap->bind();
		m_shaderChain->setTexture("environmentMap", m_environmentMap, 0);

		cubeBufferArray->bind();
		m_frameBuffer->bind();

		Vector3 Zero;
		for (Int32 i = 0; i < 6; i++)
		{
			m_frameBuffer->addTextureCubeAttachment(COLOR_ATTACHMENT, CubeTexture, i);

			renderer->Clear(ERenderBufferBit::COLOR_BUFFER_BIT | ERenderBufferBit::DEPTH_BUFFER_BIT);

			// Draw for each direction
			MathOps::LookAt(vertexBufferData[1], Zero, targets[i], ups[i]);
			drawBufferData->bindAndRefresh();

			_gameContext->getRenderer()->DrawArray(TOPOLOGY_TRIANGLE, 0, 36);
		}

		m_shaderChain->unbind();
		m_frameBuffer->unbind();

		// Cleaning Up assets
		drawBufferData->release();

		renderer->SetRenderRasterizerState(DefaultRasterizerState::GetGPUState());

		return true;
	}

	ZE::Handle EMToIrradianceMap::ConvertToIrradianceMap(GameContext* _gameContext, IGPUTexture* environmentMap)
	{
		ScopedRenderThreadOwnership renderLock(_gameContext->getRenderer());
		EMToIrradianceMap* instance = EMToIrradianceMap::GetInstance();
		instance->m_environmentMap = environmentMap;
		instance->prepare(_gameContext);
		bool success = instance->Execute(_gameContext);
		instance->release(_gameContext);

		return success ? instance->m_result : Handle();
	}

}

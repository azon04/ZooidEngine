#include "EquiRectangularToCubeMap.h"
#include "ResourceManagers/ShaderManager.h"
#include "Renderer/IRenderer.h"
#include "Renderer/IGPUTexture.h"
#include "Renderer/IGPUFrameBuffer.h"
#include "Renderer/IGPURenderBuffer.h"
#include "Renderer/IGPUStates.h"
#include "Renderer/RenderZooid.h"
#include "Renderer/BufferData.h"
#include "Resources/Texture.h"
#include "ResourceManagers/BufferManager.h"
#include "Math/MathOps.h"
#include "ZEGameContext.h"
#include "SharedResources.h"

namespace ZE
{
	void EquiRectangularToCubeMap::prepare(GameContext* _gameContext)
	{
		// Load Shader
		if (!m_shaderChain)
		{
			m_shaderChain = ShaderManager::GetInstance()->makeShaderChain("ZooidEngine/Shaders/Utils/EquiRectangularToCube.vs", "ZooidEngine/Shaders/Utils/EquiRectangularToCube.frag", nullptr, nullptr);
		}
	}

	void EquiRectangularToCubeMap::release(GameContext* _gameContext)
	{

	}

	bool EquiRectangularToCubeMap::execute_CPU(GameContext* _gameContext)
	{
		return true;
	}

	bool EquiRectangularToCubeMap::execute_GPU(GameContext* _gameContext)
	{
		ZCHECK(m_equiRectangularTexture);

		const Int32 TextureSize = 1024;
		IRenderer* renderer = _gameContext->getRenderer();

		// Create the cube texture result
		TextureCreateDesc cubeTextureDesc;
		cubeTextureDesc.Width = cubeTextureDesc.Height = TextureSize;
		cubeTextureDesc.FaceCount = 6;
		cubeTextureDesc.WrapU = cubeTextureDesc.WrapV = CLAMP_TO_EDGE;
		cubeTextureDesc.MinFilter = LINEAR;
		cubeTextureDesc.MagFilter = LINEAR;
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

		// Create Depth Buffer
		Handle depthRenderBufferHandle = _gameContext->getRenderZooid()->CreateRenderBuffer();
		ZCHECK(depthRenderBufferHandle.isValid())
		IGPURenderBuffer* depthRenderBuffer = depthRenderBufferHandle.getObject<IGPURenderBuffer>();
		depthRenderBuffer->create(DEPTH_ONLY, TextureSize, TextureSize);

		// Create the frame buffer
		Handle fbHandle = _gameContext->getRenderZooid()->CreateFrameBuffer();
		ZCHECK(fbHandle.isValid());

		IGPUFrameBuffer* frameBuffer = fbHandle.getObject<IGPUFrameBuffer>();

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
		renderer->SetViewport(0, 0, TextureSize, TextureSize);

		frameBuffer->bind();
		frameBuffer->addRenderBufferAttachment(DEPTH_ATTACHMENT, depthRenderBuffer);

		m_shaderChain->bind();

		drawBufferData->bind();
		m_shaderChain->bindConstantBuffer("draw_data", drawBufferData);

		m_equiRectangularTexture->bind();
		m_shaderChain->setTexture("equirectangularMap", m_equiRectangularTexture, 0);

		cubeBufferArray->bind();
		frameBuffer->bind();

		Vector3 Zero;
		for (Int32 i = 0; i < 6; i++)
		{
			frameBuffer->addTextureCubeAttachment(COLOR_ATTACHMENT, CubeTexture, i);

			renderer->Clear(ERenderBufferBit::COLOR_BUFFER_BIT | ERenderBufferBit::DEPTH_BUFFER_BIT);

			// Draw for each direction
			MathOps::LookAt(vertexBufferData[1], Zero, targets[i], ups[i]);
			drawBufferData->bindAndRefresh();

			_gameContext->getRenderer()->DrawArray(TOPOLOGY_TRIANGLE, 0, 36);
		}
		
		m_shaderChain->unbind();
		frameBuffer->unbind();

		// Cleaning Up assets
		frameBuffer->release();
		depthRenderBuffer->release();
		drawBufferData->release();

		renderer->SetRenderRasterizerState(DefaultRasterizerState::GetGPUState());

		return true;
	}

	ZE::Handle EquiRectangularToCubeMap::ConvertToCubeMap(GameContext* _gameContext, IGPUTexture* equiRectangularTexture)
	{
		ScopedRenderThreadOwnership renderLock(_gameContext->getRenderer());
		EquiRectangularToCubeMap* instance = EquiRectangularToCubeMap::GetInstance();
		instance->m_equiRectangularTexture = equiRectangularTexture;
		instance->prepare(_gameContext);
		bool success = instance->Execute(_gameContext);
		instance->release(_gameContext);

		return success ? instance->m_result : Handle();
	}

}


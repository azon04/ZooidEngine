#include "LightRenderPass.h"
#include "Renderer/IRenderer.h"
#include "ResourceManagers/ShaderManager.h"
#include "ResourceManagers/BufferManager.h"
#include "Resources/Texture.h"
#include "Renderer/RenderZooid.h"
#include "Renderer/IGPUTexture.h"
#include "Renderer/IGPUFrameBuffer.h"
#include "Renderer/IGPUBufferArray.h"
#include "Renderer/IGPURenderBuffer.h"
#include "Renderer/IGPUStates.h"
#include "Renderer/DrawList.h"
#include "SceneRenderer/ShadowRenderer.h"
#include "SceneRenderer/SkyboxRenderer.h"
#include "ZEGameContext.h"

ZE::LightRenderPass::LightRenderPass()
	: m_shaderChain(nullptr), m_resultPassTexture(nullptr), m_resultFrameBuffer(nullptr)
{}

void ZE::LightRenderPass::prepare(GameContext* _gameContext)
{
	ScopedRenderThreadOwnership renderLock(_gameContext->getRenderer());

	// Load shader for this pass
	if (!m_shaderChain)
	{
		m_shaderChain = ShaderManager::GetInstance()->makeShaderChain("ZooidEngine/Shaders/DeferredShading/DeferredLightShader.vs", "ZooidEngine/Shaders/DeferredShading/DeferredLightShader.frag", nullptr, nullptr);
		
#if RENDER_LIGHT_PASS_ALG == RENDER_LIGHT_PASS_LIGHT_INDEXED
		m_lightIndexedVolumeShader = ShaderManager::GetInstance()->makeShaderChain("ZooidEngine/Shaders/DeferredShading/LightIndexedVolumeShader.vs", "ZooidEngine/Shaders/DeferredShading/LightIndexedVolumeShader.frag", nullptr, nullptr);
#elif RENDER_LIGHT_PASS_ALG == RENDER_LIGHT_PASS_PER_TYPE
		m_directionalShader = ShaderManager::GetInstance()->makeShaderChain("ZooidEngine/Shaders/DeferredShading/DeferredLightShader_Directional.vs", "ZooidEngine/Shaders/DeferredShading/DeferredLightShader_Directional.frag", nullptr, nullptr);
		m_spotLightShader = ShaderManager::GetInstance()->makeShaderChain("ZooidEngine/Shaders/DeferredShading/DeferredLightShader_Spot.vs", "ZooidEngine/Shaders/DeferredShading/DeferredLightShader_Spot.frag", nullptr, nullptr);
		m_pointLightShader = ShaderManager::GetInstance()->makeShaderChain("ZooidEngine/Shaders/DeferredShading/DeferredLightShader_Point.vs", "ZooidEngine/Shaders/DeferredShading/DeferredLightShader_Point.frag", nullptr, nullptr);
#endif
	}

	// Create Frame Buffer
	if (!m_resultFrameBuffer)
	{
		TextureCreateDesc textureCreateDesc;
		textureCreateDesc.Width = _gameContext->getRenderer()->GetWidth();
		textureCreateDesc.Height = _gameContext->getRenderer()->GetHeight();
		textureCreateDesc.MinFilter = NEAREST;
		textureCreateDesc.MagFilter = NEAREST;
		textureCreateDesc.TextureFormat = TEX_RGB16F;
		textureCreateDesc.DataType = UNSIGNED_BYTE;
		textureCreateDesc.bGenerateMipMap = false;

		// Result Pass Texture Buffer
		Handle textureHandle = _gameContext->getRenderZooid()->CreateRenderTexture();
		if (textureHandle.isValid())
		{
			m_resultPassTexture = textureHandle.getObject<IGPUTexture>();
			m_resultPassTexture->create(textureCreateDesc);
		}

		// Depth Render Buffer
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
			m_resultFrameBuffer->addTextureAttachment(COLOR_ATTACHMENT, m_resultPassTexture);
			m_resultFrameBuffer->addRenderBufferAttachment(DEPTH_ATTACHMENT, m_depthRenderBuffer);
			m_resultFrameBuffer->setupAttachments();
			m_resultFrameBuffer->unbind();
		}

#if RENDER_LIGHT_PASS_ALG == RENDER_LIGHT_PASS_LIGHT_INDEXED
		// Light Volume Index Buffer
		textureCreateDesc.TextureFormat = TEX_RGBA8;
		textureCreateDesc.DataType = UNSIGNED_BYTE;
		textureCreateDesc.bGenerateMipMap = false;

		Handle hLightTexture = _gameContext->getRenderZooid()->CreateRenderTexture();
		if (hLightTexture.isValid())
		{
			m_lightVolumeIndexTexture = hLightTexture.getObject<IGPUTexture>();
			m_lightVolumeIndexTexture->create(textureCreateDesc);
		}

		// Create Frame Buffer
		Handle lightFbHandle = _gameContext->getRenderZooid()->CreateFrameBuffer();
		if (lightFbHandle.isValid())
		{
			m_lightVolumeFrameBuffer = lightFbHandle.getObject<IGPUFrameBuffer>();
			m_lightVolumeFrameBuffer->bind();
			m_lightVolumeFrameBuffer->addTextureAttachment(COLOR_ATTACHMENT, m_lightVolumeIndexTexture);
			m_lightVolumeFrameBuffer->addRenderBufferAttachment(DEPTH_ATTACHMENT, m_depthRenderBuffer);
			m_lightVolumeFrameBuffer->setupAttachments();
			m_lightVolumeFrameBuffer->unbind();
		}
#elif RENDER_LIGHT_PASS_ALG == RENDER_LIGHT_PASS_PER_TYPE
		m_lightSampleGPUBuffer = BufferManager::getInstance()->createConstantBuffer(&m_lightSampleData, sizeof(LightSampleData), CONSTANT_BUFFER_LIGHT_DATA_INDEX);
#endif

		m_sphereBufferArray = BufferManager::getInstance()->getBufferArray(BUFFER_ARRAY_SPHERE);
		m_coneBufferArray = BufferManager::getInstance()->getBufferArray(BUFFER_ARRAY_CONE);
	}
}

void ZE::LightRenderPass::release(GameContext* _gameContext)
{
	if (m_resultFrameBuffer) { m_resultFrameBuffer->release(); m_resultFrameBuffer = nullptr; }
	if (m_resultPassTexture) { m_resultPassTexture->release(); m_resultPassTexture = nullptr; }
}

void ZE::LightRenderPass::begin(GameContext* _gameContext)
{
	RenderPass::begin(_gameContext);
}

void ZE::LightRenderPass::end(GameContext* _gameContext)
{
	RenderPass::end(_gameContext);
	addOutputTextureBuffer(m_resultPassTexture);
	addOutputFrameBuffer(m_resultFrameBuffer);
}

bool ZE::LightRenderPass::execute_CPU(GameContext* _gameContext)
{
	return true;
}

bool ZE::LightRenderPass::execute_GPU(GameContext* _gameContext)
{
	DrawList* drawList = _gameContext->getDrawList();
	IRenderer* renderer = _gameContext->getRenderer();

	// Copy Depth Texture from Input Frame Buffer
	IGPUFrameBuffer* inputFrameBuffer = m_frameBufferInputs[0];
	ZCHECK(inputFrameBuffer);

	Int32 width = _gameContext->getRenderer()->GetWidth();
	Int32 height = _gameContext->getRenderer()->GetHeight();

	// Bind Textures
	ZCHECK(m_textureBufferInputs.length() == 6);
	IGPUTexture* positionTexture = m_textureBufferInputs[0];
	IGPUTexture* normalTexture = m_textureBufferInputs[1];
	IGPUTexture* albedoTexture = m_textureBufferInputs[2];
	IGPUTexture* specTexture = m_textureBufferInputs[3];
	IGPUTexture* ambientTexture = m_textureBufferInputs[4];
	IGPUTexture* ssaoTexture = m_textureBufferInputs[5];

#if RENDER_LIGHT_PASS_ALG == RENDER_LIGHT_PASS_PER_TYPE
	// Deferred Shading with Light Type
	ZCHECK(m_resultFrameBuffer);
	m_resultFrameBuffer->bind();

	// Clear Bit
	renderer->ClearScreen();

	// Copy Depth Buffer First
	_gameContext->getRenderer()->CopyFrameBuffer(inputFrameBuffer, m_resultFrameBuffer, 0, 0, width, height, 0, 0, width, height, ERenderBufferBit::DEPTH_BUFFER_BIT, ETextureFilter::NEAREST);

	// Set Blend State
	renderer->SetRenderBlendState(TRenderBlendState<true, ERendererBlendFactor::ONE, ERendererBlendFactor::ONE>::GetGPUState());

	IGPUBufferArray* quadVBO = BufferManager::getInstance()->getBufferArray(BUFFER_ARRAY_QUAD_V3_TC2);
	
	// Initialize Light Sample Data
	m_lightSampleData.viewPos = drawList->m_lightData.ViewPos;

	// For each map do render light scene
	for (Int32 lightIndex = 0; lightIndex < drawList->m_lightData.NumLight; lightIndex++)
	{
		LightStruct& light = drawList->m_lightData.lights[lightIndex];
		m_lightSampleData.light = light;

		IGPUBufferArray* bufferArray = nullptr;
		IShaderChain* shader = nullptr;

		if (light.Type == 0) // Directional Light
		{
			bufferArray = BufferManager::getInstance()->getBufferArray(BUFFER_ARRAY_QUAD_V3_TC2);
			shader = m_directionalShader;

			// Set Depth and Rasterizer State for the light
			renderer->SetRenderDepthStencilState(TRenderDepthStencilState<false, false, false, ERendererCompareFunc::ALWAYS, ERendererCompareFunc::ALWAYS, 0, 0, 0>::GetGPUState());
			renderer->SetRenderRasterizerState(DefaultRasterizerState::GetGPUState());

			shader->bind();
		}
		else if(light.Type == 1) // Point Light
		{
			bufferArray = m_sphereBufferArray;
			shader = m_pointLightShader;

			// Set Depth and Rasterizer State for the light
			renderer->SetRenderDepthStencilState(TRenderDepthStencilState<true, false, false, ERendererCompareFunc::GEQUAL, ERendererCompareFunc::ALWAYS, 0, 0, 0>::GetGPUState());
			renderer->SetRenderRasterizerState(TRenderRasterizerState<EFaceFrontOrder::CCW, ECullFace::FRONT, ERenderFillMode::MODE_FILL>::GetGPUState());

			Matrix4x4 mat;
			mat.setScale(Vector3(light.Att_distance));
			mat.setPos(light.Position);

			shader->bind();

			IGPUBufferData* drawBufferData = BufferManager::getInstance()->getOrCreateDrawBuffer(mat.m_data, sizeof(Matrix4x4));
			drawBufferData->bind();
			shader->bindConstantBuffer("draw_data", drawBufferData);
		}
		else if (light.Type == 2) // Spot Light
		{
			bufferArray = m_coneBufferArray;
			shader = m_spotLightShader;

			// Set Depth and Rasterizer State for the light
			renderer->SetRenderDepthStencilState(TRenderDepthStencilState<true, false, false, ERendererCompareFunc::GEQUAL, ERendererCompareFunc::ALWAYS, 0, 0, 0>::GetGPUState());
			renderer->SetRenderRasterizerState(TRenderRasterizerState<EFaceFrontOrder::CCW, ECullFace::FRONT, ERenderFillMode::MODE_FILL>::GetGPUState());
			
			Matrix4x4 mat;
			float radius = light.Att_distance * sqrt(1 - light.OuterCutOff * light.OuterCutOff) / light.OuterCutOff; // Calculate bottom radius
			mat.setScale(Vector3(radius, light.Att_distance, radius));
			mat.setPos(light.Position);

			shader->bind();

			IGPUBufferData* drawBufferData = BufferManager::getInstance()->getOrCreateDrawBuffer(mat.m_data, sizeof(Matrix4x4));
			drawBufferData->bind();
			shader->bindConstantBuffer("draw_data", drawBufferData);
		}

		ZCHECK(shader);
		ZCHECK(bufferArray);

		// Assign shadow map to Light Sample Data
		m_lightSampleData.light.reset();
		
		Array<Int32> shadowIndices;

		Int32 shadowMapIndex = 0;
		while (light.shadowMapIndices[shadowMapIndex] != -1 && shadowMapIndex < 8)
		{
			Int32 index = light.shadowMapIndices[shadowMapIndex];
			
			m_lightSampleData.light.shadowMapIndices[shadowMapIndex] = shadowMapIndex;
			m_lightSampleData.shadowData[shadowMapIndex] = drawList->m_lightData.shadowData[index];

			shadowIndices.push_back(index);

			shadowMapIndex++;
		}

		Int32 cascadeIndex = 0;
		while (light.cascadeShadowIndices[cascadeIndex] != -1 && cascadeIndex < 4)
		{
			Int32 index = light.cascadeShadowIndices[cascadeIndex];

			m_lightSampleData.light.cascadeShadowIndices[cascadeIndex] = cascadeIndex;
			m_lightSampleData.cascadeShadowData[cascadeIndex] = drawList->m_lightData.cascadeShadowData[index];
			m_lightSampleData.cascadeShadowData[cascadeIndex].shadowMapIndex = shadowMapIndex;
			m_lightSampleData.shadowData[shadowMapIndex] = drawList->m_lightData.shadowData[index];

			shadowIndices.push_back(drawList->m_lightData.cascadeShadowData[index].shadowMapIndex);

			cascadeIndex++;
			shadowMapIndex++;
		}

		m_lightSampleGPUBuffer->bindAndRefresh();
		shader->bindConstantBuffer("light_sample_data", m_lightSampleGPUBuffer);

		_gameContext->getDrawList()->m_mainConstantBuffer->bind();
		shader->bindConstantBuffer("frame_data", _gameContext->getDrawList()->m_mainConstantBuffer);

		shader->setTexture("gPosition", positionTexture, 0);
		positionTexture->bind();

		shader->setTexture("gNormal", normalTexture, 1);
		normalTexture->bind();

		shader->setTexture("gAlbedo", albedoTexture, 2);
		albedoTexture->bind();

		shader->setTexture("gSpec", specTexture, 3);
		specTexture->bind();

		shader->setTexture("gAmbient", ambientTexture, 4);
		ambientTexture->bind();

		shader->setTexture("gSSAO", ssaoTexture, 5);
		ssaoTexture->bind();

		ShadowDepthRenderer::BindShadowTexturesIndexed(_gameContext->getDrawList(), shader, 6, shadowIndices);

		_gameContext->getRenderer()->DrawBufferArray(TOPOLOGY_TRIANGLE, bufferArray, bufferArray->getDataCount());

		shader->unbind();
	}

	SkyBoxRenderer::Render(drawList->m_skyboxRenderGatherer.getRenderInfos(), drawList->m_skyboxRenderGatherer.getRenderCount());

	m_resultFrameBuffer->unbind();

	// Reset Render State
	renderer->SetRenderRasterizerState(DefaultRasterizerState::GetGPUState());
	renderer->SetRenderDepthStencilState(DefaultDepthStencilState::GetGPUState());
	renderer->SetRenderBlendState(DefaultRenderBlendState::GetGPUState());

#elif RENDER_LIGHT_PASS_ALG == RENDER_LIGHT_PASS_LIGHT_INDEXED // #TODO WIP
	
	// Render Light Volume to texture
	ZCHECK(m_lightVolumeFrameBuffer);
	m_lightVolumeFrameBuffer->bind();

	_gameContext->getRenderer()->CopyFrameBuffer(inputFrameBuffer, m_lightVolumeFrameBuffer, 0, 0, width, height, 0, 0, width, height, ERenderBufferBit::DEPTH_BUFFER_BIT, ETextureFilter::NEAREST);
	
	renderer->Clear(ERenderBufferBit::COLOR_BUFFER_BIT);

	// Config Render State
	renderer->SetRenderRasterizerState(TRenderRasterizerState<EFaceFrontOrder::CCW, ECullFace::FRONT, ERenderFillMode::MODE_FILL>::GetGPUState());
	renderer->SetRenderDepthStencilState(TRenderDepthStencilState<true, false, false, ERendererCompareFunc::GEQUAL, ERendererCompareFunc::ALWAYS, 0xFF, 0xFF, 0xFF>::GetGPUState());
	renderer->SetRenderBlendState(TRenderBlendState<true, ERendererBlendFactor::ONE, ERendererBlendFactor::CONSTANT_COLOR>::GetGPUState());
	renderer->SetBlendColorRef(0.251f, 0.251f, 0.251f, 0.251f);

	m_lightIndexedVolumeShader->bind();

	_gameContext->getDrawList()->m_mainConstantBuffer->bind();
	m_lightIndexedVolumeShader->bindConstantBuffer("frame_data", _gameContext->getDrawList()->m_mainConstantBuffer);

	// Draw Light
	for(Int32 i=0; i < _gameContext->getDrawList()->m_lightData.NumLight; i++)
	{
		drawLightVolume(i, renderer, _gameContext);
	}
	m_lightIndexedVolumeShader->unbind();


	m_lightVolumeFrameBuffer->unbind();

	// Reset Render State
	renderer->SetRenderRasterizerState(DefaultRasterizerState::GetGPUState());
	renderer->SetRenderDepthStencilState(DefaultDepthStencilState::GetGPUState());
	renderer->SetRenderBlendState(DefaultRenderBlendState::GetGPUState());

	// Deferred Shading with Light
	ZCHECK(m_resultFrameBuffer);
	m_resultFrameBuffer->bind();

	IGPUBufferArray* quadVBO = BufferManager::getInstance()->getBufferArray(BUFFER_ARRAY_QUAD_V3_TC2);

	m_shaderChain->bind();

	_gameContext->getDrawList()->m_lightConstantBuffer->bind();
	m_shaderChain->bindConstantBuffer("light_data", _gameContext->getDrawList()->m_lightConstantBuffer);

	_gameContext->getDrawList()->m_mainConstantBuffer->bind();
	m_shaderChain->bindConstantBuffer("frame_data", _gameContext->getDrawList()->m_mainConstantBuffer);

	m_shaderChain->setTexture("gPosition", positionTexture, 0);
	positionTexture->bind();

	m_shaderChain->setTexture("gNormal", normalTexture, 1);
	normalTexture->bind();

	m_shaderChain->setTexture("gAlbedo", albedoTexture, 2);
	albedoTexture->bind();

	m_shaderChain->setTexture("gSpec", specTexture, 3);
	specTexture->bind();

	m_shaderChain->setTexture("gAmbient", ambientTexture, 4);
	ambientTexture->bind();

	m_shaderChain->setTexture("gSSAO", ssaoTexture, 5);
	ssaoTexture->bind();

	ShadowDepthRenderer::BindShadowTextures(_gameContext->getDrawList(), m_shaderChain, 6);

	_gameContext->getRenderer()->DrawBufferArray(TOPOLOGY_TRIANGLE, quadVBO, 6);

	m_shaderChain->unbind();

	_gameContext->getRenderer()->CopyFrameBuffer(inputFrameBuffer, m_resultFrameBuffer, 0, 0, width, height, 0, 0, width, height, ERenderBufferBit::DEPTH_BUFFER_BIT, ETextureFilter::NEAREST);

	SkyBoxRenderer::Render(drawList->m_skyboxRenderGatherer.getRenderInfos(), drawList->m_skyboxRenderGatherer.getRenderCount());

	m_resultFrameBuffer->unbind();

#else
	// Deferred Shading with Light
	ZCHECK(m_resultFrameBuffer);
	m_resultFrameBuffer->bind();

	IGPUBufferArray* quadVBO = BufferManager::getInstance()->getBufferArray(BUFFER_ARRAY_QUAD_V3_TC2);

	m_shaderChain->bind();

	_gameContext->getDrawList()->m_lightConstantBuffer->bind();
	m_shaderChain->bindConstantBuffer("light_data", _gameContext->getDrawList()->m_lightConstantBuffer);

	_gameContext->getDrawList()->m_mainConstantBuffer->bind();
	m_shaderChain->bindConstantBuffer("frame_data", _gameContext->getDrawList()->m_mainConstantBuffer);

	m_shaderChain->setTexture("gPosition", positionTexture, 0);
	positionTexture->bind();

	m_shaderChain->setTexture("gNormal", normalTexture, 1);
	normalTexture->bind();

	m_shaderChain->setTexture("gAlbedo", albedoTexture, 2);
	albedoTexture->bind();

	m_shaderChain->setTexture("gSpec", specTexture, 3);
	specTexture->bind();

	m_shaderChain->setTexture("gAmbient", ambientTexture, 4);
	ambientTexture->bind();

	m_shaderChain->setTexture("gSSAO", ssaoTexture, 5);
	ssaoTexture->bind();

	ShadowDepthRenderer::BindShadowTextures(_gameContext->getDrawList(), m_shaderChain, 6);

	_gameContext->getRenderer()->DrawBufferArray(TOPOLOGY_TRIANGLE, quadVBO, 6);

	m_shaderChain->unbind();

	_gameContext->getRenderer()->CopyFrameBuffer(inputFrameBuffer, m_resultFrameBuffer, 0, 0, width, height, 0, 0, width, height, ERenderBufferBit::DEPTH_BUFFER_BIT, ETextureFilter::NEAREST);

	SkyBoxRenderer::Render(drawList->m_skyboxRenderGatherer.getRenderInfos(), drawList->m_skyboxRenderGatherer.getRenderCount());

	m_resultFrameBuffer->unbind();
#endif

	return true;
}

#if RENDER_LIGHT_PASS_ALG == RENDER_LIGHT_PASS_LIGHT_INDEXED
void ZE::LightRenderPass::drawLightVolume(Int32 lightIndex, IRenderer* renderer, GameContext* gameContex)
{
	LightStruct& light = gameContex->getDrawList()->m_lightData.lights[lightIndex];

	if (light.Type == 0) // Skip Directional Light
	{
		return;
	}

	IGPUBufferArray* bufferArray = light.Type == 1 ? m_sphereBufferArray : m_coneBufferArray;
	Matrix4x4 mat;

	Int32 index = lightIndex + 1;
	UInt32 r = (index & (0x3 << 0)) << 6;
	UInt32 g = (index & (0x3 << 2)) << 4;
	UInt32 b = (index & (0x3 << 4)) << 2;
	UInt32 a = (index & (0x3 << 6)) << 0;

	Vector4 Color(r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f);

	if (light.Type == 1)
	{
		mat.setScale(Vector3(light.Att_distance));
	}
	else
	{
		float radius = light.Att_distance * sqrt(1 - light.OuterCutOff * light.OuterCutOff) / light.OuterCutOff;
		mat.setScale(Vector3(radius, light.Att_distance, radius));
	}
	
	mat.setPos(light.Position);

	IGPUBufferData* drawBufferData = BufferManager::getInstance()->getOrCreateDrawBuffer(mat.m_data, sizeof(Matrix4x4));
	drawBufferData->bind();
	m_lightIndexedVolumeShader->bindConstantBuffer("draw_data", drawBufferData);

	m_lightIndexedVolumeShader->setVec4("color", Color);
	
	bufferArray->bind();

	renderer->DrawArray(TOPOLOGY_TRIANGLE, 0, bufferArray->getDataCount());
}
#endif

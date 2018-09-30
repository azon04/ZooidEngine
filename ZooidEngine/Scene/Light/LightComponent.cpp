#include "LightComponent.h"

#include "ZEGameContext.h"
#include "Events/Events.h"
#include "Renderer/ShaderData.h"
#include "Renderer/IRenderer.h"
#include "Renderer/DrawList.h"
#include "Renderer/RenderZooid.h"
#include "Renderer/IGPUFrameBuffer.h"

#include "ResourceManagers/TextureManager.h"
#include "ResourceManagers/ShaderManager.h"

namespace ZE
{
	IMPLEMENT_CLASS_1(LightComponent, SceneComponent)

	LightComponent::LightComponent(GameContext* gameContext) : 
		SceneComponent(gameContext), 
		m_lightType(DIRECTIONAL_LIGHT),
		m_diffuse(1.0f, 1.0f, 1.0f),
		m_specular(0.5f, 0.5f, 0.5f),
		m_ambient(0.25f, 0.25f, 0.25f),
		m_attConstant(1.0f),
		m_attLinear(0.07f),
		m_attQuadratic(0.017f),
		m_bGenerateShadow(true),
		m_shadowMapWidth(2048),
		m_shadowMapHeight(2048)
	{}

	LightComponent::LightComponent(GameContext* gameContext, LightType _lightType) : 
		LightComponent(gameContext)
	{
		m_lightType = _lightType;
	}

	void LightComponent::setupComponent()
	{
		addEventDelegate(Event_GATHER_LIGHT, &LightComponent::handleGatherLight);
		
		m_bLightSetup = false;
		setupLight();
	}

	void LightComponent::handleGatherLight(Event* event)
	{
		LightData& lightData = m_gameContext->getDrawList()->m_lightData;
		Int32 index = lightData.NumLight++;
		LightStruct& light = lightData.lights[index];
		light.Type = m_lightType;

		light.setAmbient(m_ambient);
		light.setDiffuse(m_diffuse);
		light.setSpecular(m_specular);

		switch (m_lightType)
		{
		case ZE::DIRECTIONAL_LIGHT:
			light.setDirection(m_worldTransform.getN());
			break;
		case ZE::POINT_LIGHT:
			light.setPosition(m_worldTransform.getPos());
			light.Att_constant = m_attConstant;
			light.Att_linear = m_attLinear;
			light.Att_quadratic = m_attQuadratic;
			break;
		case ZE::SPOT_LIGHT:
			light.setPosition(m_worldTransform.getPos());
			light.setDirection(m_worldTransform.getN());
			light.Att_constant = m_attConstant;
			light.Att_linear = m_attLinear;
			light.Att_quadratic = m_attQuadratic;
			light.CutOff = m_innerRadius;
			light.OuterCutOff = m_outerRadius;
			break;
		default:
			break;
		}

		if (m_bGenerateShadow)
		{
			LightShadowMapData& shadowMapData = m_gameContext->getDrawList()->getNextLightShadowMapData();
			shadowMapData.dynamicShadowFrameBuffer = m_dynamicShadowFrameBuffer;
			shadowMapData.dynamicShadowTexture = m_dynamicShadowTexture;
			shadowMapData.staticShadowTexture = m_staticShadowTexture;
			shadowMapData.lightIndex = index;
			shadowMapData.normalShaderChain = ShaderManager::GetInstance()->getShaderChain(Z_SHADER_CHAIN_SHADOW_DEPTH);
			shadowMapData.skinnedShaderChain = ShaderManager::GetInstance()->getShaderChain(Z_SHADER_cHAIN_SHADOW_DEPTH_SKINNED);
		}
	}

	void LightComponent::setStatic(bool bStatic)
	{
		m_bStatic = bStatic;
	}

	void LightComponent::setGenerateShadow(bool bGenerateShadow)
	{
		m_bGenerateShadow = bGenerateShadow;
	}

	void LightComponent::setStaticShadowTexture(const String& path)
	{
		m_staticShadowFile = path;
	}

	void LightComponent::setShadowResolution(UInt32 _resolution)
	{
		setShadowMapWidth(_resolution);
		setShadowMapHeight(_resolution);
	}

	void LightComponent::setShadowMapWidth(UInt32 _width)
	{
		m_shadowMapWidth = _width;
	}

	void LightComponent::setShadowMapHeight(UInt32 _height)
	{
		m_shadowMapHeight = _height;
	}

	void LightComponent::setupLight()
	{
		if (m_bLightSetup && !m_bGenerateShadow)
		{
			if (m_staticShadowTexture && m_staticShadowFile.length() > 0)
			{
				TextureManager::GetInstance()->unloadResource(m_staticShadowFile.const_str());
				m_staticShadowTexture = nullptr;
			}

			if (m_dynamicShadowTexture)
			{
				m_dynamicShadowTexture->release();
				m_dynamicShadowTexture = nullptr;
			}

			return;
		}

		if (!m_bGenerateShadow) { return; }
		
		m_staticShadowTexture = nullptr;
		m_dynamicShadowTexture = nullptr;
		m_dynamicShadowFrameBuffer = nullptr;

		if (m_staticShadowFile.length() > 0)
		{
			Handle hTexture = TextureManager::GetInstance()->loadResource(m_staticShadowFile.const_str());
			if (hTexture.isValid())
			{
				m_staticShadowTexture = hTexture.getObject<IGPUTexture>();
			}
		}

		m_gameContext->getRenderer()->AcquireRenderThreadOwnership();

		// Setup Dynamic Texture
		Handle hTexture( "Texture", sizeof(Texture));
		Texture* pCPUTexture = new(hTexture) Texture();
		pCPUTexture->createEmpty(m_shadowMapWidth, m_shadowMapHeight, TEX_DEPTH);
		pCPUTexture->setGenerateMipmap(false);
		pCPUTexture->setDataType(FLOAT);
		pCPUTexture->setWrapOnU(CLAMP_TO_BORDER);
		pCPUTexture->setWrapOnV(CLAMP_TO_EDGE);
		pCPUTexture->setMinFilter(NEAREST);
		pCPUTexture->setMagFilter(NEAREST);
		pCPUTexture->setBorderColor(Vector4(1.0f, 1.0f, 1.0f, 1.0f));

		Handle hGPUTexture = m_gameContext->getRenderZooid()->CreateRenderTexture();
		if (hGPUTexture.isValid())
		{
			m_dynamicShadowTexture = hGPUTexture.getObject<IGPUTexture>();
			m_dynamicShadowTexture->fromTexture(pCPUTexture);
		}

		// Setup Frame Buffer
		Handle hFrameBuffer = m_gameContext->getRenderZooid()->CreateFrameBuffer();
		if (hFrameBuffer.isValid())
		{
			m_dynamicShadowFrameBuffer = hFrameBuffer.getObject<IGPUFrameBuffer>();
			m_dynamicShadowFrameBuffer->bind();
			m_dynamicShadowFrameBuffer->addTextureAttachment(DEPTH_ATTACHMENT, m_dynamicShadowTexture);
			m_dynamicShadowFrameBuffer->setupAttachments();
			m_dynamicShadowFrameBuffer->unbind();
		}

		m_gameContext->getRenderer()->ReleaseRenderThreadOwnership();

		m_bLightSetup = true;
	}

}
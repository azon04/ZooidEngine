#include "LightComponent.h"

#include "ZEGameContext.h"
#include "Events/Events.h"
#include "Renderer/ShaderData.h"

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
		m_attQuadratic(0.017f)
	{}

	LightComponent::LightComponent(GameContext* gameContext, LightType _lightType) : 
		LightComponent(gameContext)
	{
		m_lightType = _lightType;
	}

	void LightComponent::setupComponent()
	{
		addEventDelegate(Event_GATHER_LIGHT, &LightComponent::handleGatherLight);
	}

	void LightComponent::handleGatherLight(Event* event)
	{
		LightData& lightData = m_gameContext->getDrawList()->m_lightData;
		Int32 index = lightData.numLight++;
		LightStruct& light = lightData.lights[index];
		light.type = m_lightType;

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
			light.att_constant = m_attConstant;
			light.att_linear = m_attLinear;
			light.att_quadratic = m_attQuadratic;
			break;
		case ZE::SPOT_LIGHT:
			light.setPosition(m_worldTransform.getPos());
			light.setDirection(m_worldTransform.getN());
			light.att_constant = m_attConstant;
			light.att_linear = m_attLinear;
			light.att_quadratic = m_attQuadratic;
			light.cutOff = m_innerRadius;
			light.outerCutOff = m_outerRadius;
			break;
		default:
			break;
		}
	}

}
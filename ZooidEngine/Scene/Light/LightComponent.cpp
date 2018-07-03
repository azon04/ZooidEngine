#include "LightComponent.h"

#include "ZEGameContext.h"
#include "Events/Events.h"
#include "Renderer/ShaderData.h"
#include "Renderer/IRenderer.h"
#include "Renderer/DrawList.h"

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
	}

}
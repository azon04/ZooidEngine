#ifndef __ZE_LIGHT_COMP_H__
#define __ZE_LIGHT_COMP_H__

#include "Scene/SceneComponent.h"
#include "Math/Vector3.h"

namespace ZE
{
	enum LightType : Int32
	{
		DIRECTIONAL_LIGHT = 0,
		POINT_LIGHT = 1,
		SPOT_LIGHT = 2
	};

	class LightComponent : public SceneComponent
	{
		DEFINE_CLASS(LightComponent);

	public:
		LightComponent(GameContext* gameContext);
		LightComponent(GameContext* gameContext, LightType _lightType);

		virtual void setupComponent() override;

		// Handle gather light event
		virtual void handleGatherLight(Event* event);
		
		LightType m_lightType;

		Vector3 m_ambient;
		Vector3 m_diffuse;
		Vector3 m_specular;

		// Attenuation
		Float32 m_attConstant;
		Float32 m_attLinear;
		Float32 m_attQuadratic;

		// Spot Light Attributes
		Float32 m_innerRadius;
		Float32 m_outerRadius;
	};
}
#endif

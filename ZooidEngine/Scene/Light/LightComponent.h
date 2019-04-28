#ifndef __ZE_LIGHT_COMP_H__
#define __ZE_LIGHT_COMP_H__

#include "Scene/SceneComponent.h"
#include "Math/Vector3.h"

#define MAX_LIGHT_SHADOW_MAPS 4

namespace ZE
{
	class IGPUTexture;
	class IGPUFrameBuffer;
	class ViewFustrum;

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
		
		// Set Light as Static
		virtual void setStatic(bool bStatic);

		// Set Light generate shadow
		virtual void setGenerateShadow(bool bGenerateShadow);

		// Set Static Shadow Map texture file
		void setStaticShadowTexture(const String& path);

		// Set shadow map resolution
		void setShadowResolution(UInt32 _resolution);

		// Set Shadow map width
		void setShadowMapWidth(UInt32 _width);

		// Set Shadow map height
		void setShadowMapHeight(UInt32 _height);

		IGPUFrameBuffer* getOrCreateShadowFrameBuffer(UInt32 _index);

		IGPUTexture* getDynamicShadowMap(UInt32 _index) const { return m_dynamicShadowTextures[_index]; };

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

	protected:
		void setupLight();
		void calculateCascadeLightFustrum(Matrix4x4& view, Matrix4x4& projection, ViewFustrum* camFustrum, Float32 cascadeDistStart, Float32 cascadeDistEnd, 
			Float32 obMostRight, Float32 obMostLeft, Float32 obMostTop, Float32 obMostBottom, Float32 obMostNear );
		void setupShadowMapsDirectional(UInt32 lightIndex);
		void setupShadowMapsSpotLight(UInt32 lightIndex);
		void setupShadowMapsPointLight(UInt32 lightIndex);

	protected:
		bool m_bStatic;
		bool m_bGenerateShadow;

		IGPUTexture* m_staticShadowTexture;
		
		IGPUTexture* m_dynamicShadowTextures[MAX_LIGHT_SHADOW_MAPS];
		IGPUFrameBuffer* m_dynamicShadowFrameBuffers[MAX_LIGHT_SHADOW_MAPS];

		UInt32 m_shadowMapWidth;
		UInt32 m_shadowMapHeight;

		String m_staticShadowFile;

	private:
		bool m_bLightSetup;
	};
}
#endif

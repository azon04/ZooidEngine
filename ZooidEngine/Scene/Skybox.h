#ifndef __ZE_SKYBOX_H__
#define __ZE_SKYBOX_H__

#include "SceneComponent.h"

namespace ZE
{
	class GameContext;
	class IGPUTexture;
	class IGPUBufferArray;

	class Skybox : public SceneComponent
	{
		DEFINE_CLASS(Skybox);

	public:
		Skybox(GameContext* gameContext) : SceneComponent(gameContext) {}

		void loadFromPath(const char* filePath);
		virtual void setupComponent() override;

	protected:
		void handleGatherRender(Event* pEvent);

	protected:
		
		IGPUTexture* m_cubeMapTexture;
		static IGPUBufferArray* s_skyBoxBufferArray;

	};
}
#endif

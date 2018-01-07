#ifndef __ZE_MATERIAL_H__
#define __ZE_MATERIAL_H__

#include "ZEngineIncl.h"

namespace ZE
{
	class GPUTexture;
	class GameContext;

	class Material : public Object
	{
		DEFINE_CLASS(Material)

	public:

		Material(GameContext* gameContext) {}
		GPUTexture* m_texture;
		Float32 m_shininess;

	};
}
#endif

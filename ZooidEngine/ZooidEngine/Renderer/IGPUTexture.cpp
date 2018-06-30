#include "IGPUTexture.h"

namespace ZE
{
	void IGPUTexture::fromTexture(Texture* texture)
	{
		m_textureRes = texture;
	}

	void IGPUTexture::release()
	{
	}

}
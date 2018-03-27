#include "IGPUTexture.h"

namespace ZE
{
	void IGPUTexture::FromTexture(Texture* texture)
	{
		m_textureRes = texture;
	}

	void IGPUTexture::release()
	{
	}

}
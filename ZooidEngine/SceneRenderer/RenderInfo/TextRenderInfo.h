#ifndef __ZE_TEXT_RENDERINFO_H__
#define __ZE_TEXT_RENDERINFO_H__

#include "../RenderInfo.h"
#include "Math/Vector3.h"

namespace ZE
{
	class IGPUTexture;

	class TextRenderInfo : public RenderInfo
	{
	public:
		TextRenderInfo() :
			m_fontTexture(nullptr),
			m_color(1.0f)
		{}

		IGPUTexture* m_fontTexture;
		Vector3 m_color;
	};
}
#endif

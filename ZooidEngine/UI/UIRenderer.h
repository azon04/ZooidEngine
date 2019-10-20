#ifndef __ZE_UI_RENDERER_H__
#define __ZE_UI_RENDERER_H__

#include "ZooidUI.h"

namespace ZE
{
	class UIDrawList;

	class UIRenderer
	{
	public:
		UIRenderer() {}
		virtual ~UIRenderer() {}

		virtual void Init(int width, int height) = 0;
		virtual void ProcessCurrentDrawList() = 0;
		virtual void Destroy() = 0;
		virtual bool requestToClose() = 0;
		virtual void* getWindowContext() = 0;
		virtual UInt32 createRendererTexture(void* pAddress, UInt32 width, UInt32 height, UInt32 channelCount) = 0;
		virtual void destroyTexture(UInt32 textureHandle) = 0;
		virtual void destroyTextures() = 0;
		virtual void resizeWindow(int width, int height) = 0;
		void setDrawList(UIDrawList* _drawList) { m_drawList = _drawList; }

	protected:
		UIDrawList* m_drawList;
	};
}

#endif // 

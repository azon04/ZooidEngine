#ifndef __Z_RENDERER__
#define __Z_RENDERER__

namespace ZE {

class IRenderer {
public:
	IRenderer() {}
	virtual ~IRenderer() {}

	virtual void Setup() = 0;
	virtual void BeginRender() = 0;
	virtual void EndRender() = 0;
	virtual void Clean() = 0;
};

}
#endif

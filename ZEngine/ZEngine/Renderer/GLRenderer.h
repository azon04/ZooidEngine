#ifndef __ZE_GL_RENDERER__
#define __ZE_GL_RENDERER__

#include "IRenderer.h"

namespace ZE {
class GLRenderer : public IRenderer {
	
public:
	GLRenderer() {}
	virtual ~GLRenderer() {}

	virtual void Setup() override;
	virtual void BeginRender() override;
	virtual void EndRender() override;
	virtual void Clean() override;

};
}
#endif

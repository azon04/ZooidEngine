#ifndef __ZE_GAME_CONTEXT__
#define __ZE_GAME_CONTEXT__

#include "ZEngine.h"

namespace ZE {
	class GameContext {
	public:
		GameContext() {}
		virtual ~GameContext() {
			if (m_renderer) delete m_renderer;
		}

		IRenderer* m_renderer;
		BufferManager* m_bufferManager;

		IRenderer* getRenderer() const { return m_renderer; }
		BufferManager* getBufferManager() const { return m_bufferManager; }
	};
}
#endif // __ZE_GAME_CONTEXT__

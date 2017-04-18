#ifndef __ZE_DRAW_LIST__
#define __ZE_DRAW_LIST__

#include "ShaderAction.h"

#define MAX_DRAW_LIST 1024

namespace ZE {
	class DrawList {
	public:
		
		DrawList() {
			m_size = 0;
		}

		ShaderAction& getNextShaderAction() {
			return m_drawList[m_size++];
		}

		ShaderAction m_drawList[MAX_DRAW_LIST];
		int m_size;
	};

};
#endif // 

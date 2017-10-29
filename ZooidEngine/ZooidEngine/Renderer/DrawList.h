#ifndef __ZE_DRAW_LIST__
#define __ZE_DRAW_LIST__

#include "../Utils/PrimitiveTypes.h"
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
		ZE::Int32 m_size;
	};

};
#endif // 

#include "DrawList.h"

namespace ZE {

	void DrawList::Reset()
	{
		for (int i = 0; i < m_size; i++)
		{
			m_drawList[i].Reset();
		}

		m_size = 0;
	}

}
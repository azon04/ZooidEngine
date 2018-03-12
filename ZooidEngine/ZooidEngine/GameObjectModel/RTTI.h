#ifndef __ZE_RTTI_H__
#define __ZE_RTTI_H__

#include "../Utils/PrimitiveTypes.h"

namespace ZE {
	
	struct RTTI_Properties {

	};

	struct RTTI {
		char m_className[32];
		Int32 m_classID = -1;
		RTTI* m_parentRTTI = nullptr;

		bool IsA(Int32 classID) const
		{
			if (this->m_classID == classID)
				return true;

			RTTI* current = m_parentRTTI;
			while (current)
			{
				if (current->m_classID == classID)
				{
					return true;
				}
				current = current->m_parentRTTI;
			}

			return false;
		}
	};

}
#endif

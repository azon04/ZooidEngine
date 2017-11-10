#ifndef __ZE_RTTI_H__
#define __ZE_RTTI_H__

#include "../Utils/PrimitiveTypes.h"

namespace ZE {
	
	struct RTTI_Properties {

	};

	struct RTTI {
		char m_className[32];
		Int32 m_classID = -1;
		RTTI* m_parentRTTI;
	};

}
#endif

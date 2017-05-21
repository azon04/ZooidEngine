#ifndef __ZE_RTTI_H__
#define __ZE_RTTI_H__

namespace ZE {
	
	struct RTTI_Properties {

	};

	struct RTTI {
		char m_className[32];
		int m_classID;
		RTTI* m_parentRTTI;
	};

}
#endif

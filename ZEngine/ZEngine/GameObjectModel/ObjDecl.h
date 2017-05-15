#ifndef __ZE__OBJ_DECL_H__
#define __ZE__OBJ_DECL_H__

#define DEFINE_CLASS(ClassName) \
	public: \
	struct RTTI_ ## ClassName : public RTTI {}; \
	static RTTI_ ## ClassName m_metaInfo; \
	static void RegisterClass(); \
	virtual int getClassID() { return ClassName ## ::m_metaInfo.m_classID; }\
	virtual const char* getClassName() { return #ClassName ; } \
	static int GetClassID() { return m_metaInfo.m_classID; } \
	static int GetClassName() { return m_metaInfo.m_className; }

#define REGISTER_CLASS0(ClassName) \
	void ClassName ## ::RegisterClass() { \
		m_metaInfo.m_classID = Object::ClassMap.size(); \
		Object::ClassMap.push_back(m_metaInfo.m_classID); \
	}

#define REGISTER_CLASS1(ClassName, ParentName) \
	void ClassName ## ::RegisterClass() {\
		m_metaInfo.m_classID = Object::ClassMap.size(); \
		Object::ClassMap.push_back(m_metaInfo.m_classID); \
		m_metaInfo.m_parentRTTI = (RTTI*) & ## ClassName ## ::m_metaInfo; \
	} 

#define IMPLEMENT_CLASS_1(ClassName, ParentName) \
	REGISTER_CLASS1(ClassName, ParentName)

#define IMPLEMENT_CLASS_0(Class) \
	REGISTER_CLASS0(Class)

#endif


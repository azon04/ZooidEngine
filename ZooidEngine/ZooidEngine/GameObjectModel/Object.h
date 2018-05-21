#ifndef __ZE_OBJECT_H__
#define __ZE_OBJECT_H__

#include "Utils/PrimitiveTypes.h"
#include "ObjDecl.h"
#include "RTTI.h"
#include "Utils/String.h"

namespace ZE {

	template<class T, bool resizable>
	class Array;

	class Object {
	public:
		Object() {}
		virtual ~Object();

		virtual int getClassID() = 0;
		virtual const char* getClassName() = 0;

		static Array<Int32, true>& ClassMap();

		static void RegisterClass() {};

		void setObjectName(String name) { m_name = name; }
		String getObjectName() const { return m_name; }

	protected:

		String m_name;

	};

	
};

#endif

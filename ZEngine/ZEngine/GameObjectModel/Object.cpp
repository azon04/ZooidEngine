
#include "Object.h"
#include "../Utils/Array.h"



ZE::Array<ZE::Int32, true>& ZE::Object::ClassMap()
{
	static ZE::Array<ZE::Int32, true> classMap;
	return classMap;
}

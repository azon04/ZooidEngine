
#include "Object.h"
#include "Utils/Array.h"
#include "Memory/MemoryManager.h"

ZE::Object::~Object()
{}

ZE::Array<ZE::Int32, true>& ZE::Object::ClassMap()
{
	static ZE::Array<ZE::Int32, true> classMap;
	return classMap;
}

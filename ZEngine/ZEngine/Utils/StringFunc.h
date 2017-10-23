#ifndef __ZE_STRING_FUNC_H__
#define __ZE_STRING_FUNC_H__

#include "PrimitiveTypes.h"

#define STRING_FUNC_BUFFER_SIZE 255

namespace StringFunc {
	
	extern char Buffer[STRING_FUNC_BUFFER_SIZE];

	void WriteTo(char* to, const char* from, unsigned int size);

	int Length(char* string);

	ZE::UInt32 Hash(const char* string, size_t size);

};
#endif 

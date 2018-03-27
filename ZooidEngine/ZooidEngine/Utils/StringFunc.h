#ifndef __ZE_STRING_FUNC_H__
#define __ZE_STRING_FUNC_H__

#include "PrimitiveTypes.h"

#define STRING_FUNC_BUFFER_SIZE 255

namespace StringFunc {
	
	extern char Buffer[STRING_FUNC_BUFFER_SIZE];

	void WriteTo(char* to, const char* from, unsigned int size);

	// Length of char* string, including null terminated char
	int Length(const char* string);

	int Compare(const char* string1, const char* string2);

	void Concat(const char* string1, const char* string2, char* res);

	ZE::UInt32 Hash(const char* string, size_t size);

};
#endif 

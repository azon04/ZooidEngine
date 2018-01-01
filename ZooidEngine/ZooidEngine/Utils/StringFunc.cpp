#include "StringFunc.h"
#include <cstdlib>
#include <cstring>

extern "C"
{
	#include "../External/Hash/lookup3.h"
}

#define USING_MEM_FUNCTION 1
#define USING_ENDIANNES 1

void StringFunc::WriteTo(char* to, const char* from, unsigned int size)
{
#if USING_MEM_FUNCTION
	memcpy(to, from, size);
#else
	int index = 0;
	while (index < size-1 && from[index] != '\0')
	{
		to[index] = from[index];
	}
	to[index] = '\0';
#endif
}

int StringFunc::Length(const char* string)
{
	int length = 0;
	while(string[length++] != '\0') {}
	return length;
}

ZE::UInt32 StringFunc::Hash(const char* string, size_t size)
{
	ZE::UInt32 result;

	uint32_t initval = 13; // Prime is good
#if USING_ENDIANNES
	if (HASH_LITTLE_ENDIAN)
	{
		result = (ZE::UInt32) (hashlittle(string, size, initval));
	}
	else if (HASH_BIG_ENDIAN)
	{
		result = (ZE::UInt32) (hashbig(string, size, initval));
	}
	else
	{
		result = (ZE::UInt32) (hashword((const uint32_t *)string, size / 4, initval));
	}
#else
	result = (ZE::UInt32) (hashword((const uint32_t *)string, size / 4, initval));
#endif

	return result;
}

char StringFunc::Buffer[STRING_FUNC_BUFFER_SIZE];

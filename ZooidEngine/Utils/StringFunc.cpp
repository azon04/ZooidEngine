#include "StringFunc.h"

#include <cstdlib>
#include <cstring>
#include <stdio.h>
#include <stdarg.h>

extern "C"
{
	#include "External/Hash/lookup3.h"
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
	return length-1;
}

int StringFunc::Compare(const char* string1, const char* string2)
{
#if 0
	int length = 0;
	while (string1[length] != '\0' && string2[length] != '\0' && string1[length] == string2[length])
	{
		++length;
	}

	return string1[length] == string2[length] ? 0 : (string1[length] < string2[length] ? -1 : 1);
#else
	return strcmp(string1, string2);
#endif
}

void StringFunc::Concat(const char* string1, const char* string2, char* res)
{
	sprintf(res, "%s%s", string1, string2);
}

void StringFunc::PrintToString(char* string, int stringSize, const char* formatText, ...)
{
	char szBuff[1024];
	va_list arg;
	va_start(arg, formatText);
	_vsnprintf_s(szBuff, sizeof(szBuff), formatText, arg);
	va_end(arg);

	StringFunc::WriteTo(string, szBuff, stringSize);
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

#include "StringFunc.h"
#include <cstdlib>
#include <cstring>

#define  USING_MEM_FUNCTION 1

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

int StringFunc::Length(char* string)
{
	int length = 0;
	while(string[length++] != '\0') {}
	return length;
}

char StringFunc::Buffer[STRING_FUNC_BUFFER_SIZE];

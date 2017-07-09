#ifndef __ZE_STRING_FUNC_H__
#define __ZE_STRING_FUNC_H__

#define STRING_FUNC_BUFFER_SIZE 255

namespace StringFunc {
	
	extern char Buffer[STRING_FUNC_BUFFER_SIZE];

	void WriteTo(char* to, const char* from, unsigned int size);

	int Length(char* string);
};
#endif 

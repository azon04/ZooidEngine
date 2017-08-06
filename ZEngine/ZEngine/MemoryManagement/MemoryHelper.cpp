#include "MemoryHelper.h"
#include <cstring>

void ZE::MemoryHelper::Copy(void* source, void* dest, size_t size)
{
	std::memcpy(dest, source, size);
}

void ZE::MemoryHelper::Move(void* source, void* dest, size_t size)
{
	std::memmove(dest, source, size);
}

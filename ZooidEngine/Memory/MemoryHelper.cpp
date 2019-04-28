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

void ZE::MemoryHelper::Zero(void* dest, size_t size)
{
	std::memset(dest, 0, size);
}

void ZE::MemoryHelper::CopyTexture(void* destTexMemory, Int32 destWidth, Int32 destHeight, Int32 targetX, Int32 targetY, void* srcTexMemory, Int32 srcWidth, Int32 srcHeight, size_t itemSize)
{
	void* pDest =  (void*)(reinterpret_cast<uintptr_t>(destTexMemory) + (itemSize * destWidth * targetY) + (targetX * itemSize));
	void* pSource = srcTexMemory;
	for (Int32 i = 0; i < srcHeight; i++)
	{
		Copy(pSource, pDest, itemSize * srcWidth);
		pDest = (void*)(reinterpret_cast<uintptr_t>(pDest) + (itemSize * destWidth));
		pSource = (void*)(reinterpret_cast<uintptr_t>(pSource) + (itemSize * srcWidth));
	}
}


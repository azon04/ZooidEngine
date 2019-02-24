#include "IGPUStates.h"
#include "Memory/Handle.h"

namespace ZE
{
	ZE::Array<IGPUState*> ZE::IGPUState::s_gpuStateCollections;

	void IGPUState::ClearGPUStates()
	{
		for (UInt32 i = 0; i < s_gpuStateCollections.size(); i++)
		{
			s_gpuStateCollections[i]->release();
		}
		s_gpuStateCollections.clear();
	}

	void IGPUState::AddGPUState(IGPUState* state)
	{
		s_gpuStateCollections.push_back(state);
	}

}


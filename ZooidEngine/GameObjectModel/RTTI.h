#ifndef __ZE_RTTI_H__
#define __ZE_RTTI_H__

#include "Utils/PrimitiveTypes.h"

namespace ZE 
{
	
	struct RTTI_Properties 
	{

	};

	struct RTTI 
	{
		Int32 ClassID = -1;
		RTTI* ParentRTTI = nullptr;

		bool IsA(Int32 classID) const
		{
			if (this->ClassID == classID)
				return true;

			RTTI* current = ParentRTTI;
			while (current)
			{
				if (current->ClassID == classID)
				{
					return true;
				}
				current = current->ParentRTTI;
			}

			return false;
		}
	};

}
#endif

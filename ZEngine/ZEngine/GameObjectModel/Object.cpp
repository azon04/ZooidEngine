
#include "Object.h"

namespace ZE {
	std::vector<int>& Object::ClassMap(){
		static std::vector<Int32> impl;
		return impl;
	};

}
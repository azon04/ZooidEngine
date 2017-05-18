
#include "Object.h"


namespace ZE {
	std::vector<int>& Object::ClassMap(){
		static std::vector<int> impl;
		return impl;
	};

}
#ifndef  __ZE_HASHMAP__
#define __ZE_HASHMAP__

#include "Array.h"
#include "String.h"

namespace ZE {

	template<class K, class V>
	struct HashKeyValue
	{
		K m_key;
		uint32_t m_hashKey;
		V m_value;
	};

	template<class K, class V>
	struct HashMap : public Array<HashKeyValue<K,V>, true>
	{

	};

	template<class V>
	struct HashMap<String, V> : public Array<HashKeyValue<String, V>, true>
	{

	};

}
#endif // ! __ZE_HASHMAP__

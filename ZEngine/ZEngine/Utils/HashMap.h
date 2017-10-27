#ifndef  __ZE_HASHMAP__
#define __ZE_HASHMAP__

#include "Array.h"
#include "String.h"

#define HASH_MAP_LINEAR_PROBING 1
#define HASH_MAP_QUADRATIC_PROBING 2

#define  HASH_MAP_PROBING HASH_MAP_QUADRATIC_PROBING

namespace ZE {

	template<class K, class V>
	struct HashKeyValue
	{
		bool m_occupied = false;
		K m_key;
		uint32_t m_hashKey;
		V m_value;
	};

	template<class K>
	class HashFunc
	{
		ZE::UInt32 operator()(K& Object)
		{
			return 0;
		}
	};

	template<>
	class HashFunc<String>
	{
		ZE::UInt32 operator()(String& object)
		{
			return StringFunc::Hash(object.c_str(), StringFunc::Length(object.c_str()));
		}
	};

	template<>
	class HashFunc<ZE::Int32>
	{
		ZE::UInt32 operator()(ZE::Int32& object)
		{
			return object;
		}
	};

	template<>
	class HashFunc<const char *>
	{
		ZE::UInt32 operator()(const char* object)
		{
			return StringFunc::Hash(object, StringFunc::Length(object));
		}
	};

	template<class K, class V, class Func = HashFunc<K>>
	class HashMap : public Array<HashKeyValue<K,V>, true>
	{

	public:
		bool put_internal(const K& key, const V& value)
		{
			ZE::UInt32 hashValue = Func(key);
			int initial_index = hashValue % m_capacity;
			int index = initial_index;

			// TODO Check if the index currently occupied
			// TODO Probing until find empty spot
			// Question? How to check if the spot is empty
#if HASH_MAP_PROBING == HASH_MAP_QUADRATIC_PROBING
			int q_offset = 1;
#endif
			while (get(index).m_occupied)
			{
				// Try to reposition the current index for better performance
				HashKeyValue<String, V>& hashKeyValue = get(index);
				hashKeyValue.m_occupied = false;
				m_length--;
				put_internal(hashKeyValue.m_key, hashKeyValue.m_value);

				if (!get(index).m_occupied) { break; }

#if HASH_MAP_PROBING == HASH_MAP_LINEAR_PROBING
				index = (index + 1) % m_capacity;
#elif HASH_MAP_PROBING == HASH_MAP_QUADRATIC_PROBING
				index = (initial_index + (q_offset * q_offset)) % m_capacity;
				q_offset++;
#endif
			}

			HashKeyValue<K, V>& hashKeyValue = get(index);
			hashKeyValue.m_occupied = true;
			m_length++;
			hashKeyValue.m_hashKey = hashValue;
			hashKeyValue.m_key = key;
			hashKeyValue.m_value = value;

			return true;
		}

		bool put(const K& key, const V& value)
		{
			int index = 0;
			if (hasKey_internal(key, index))
			{
				HashKeyValue<K, V>& hashKeyValue = get(index);
				hashKeyValue.m_value = value;
				return true;
			}

			if (m_capacity == 0)
			{
				reset(1);
			}
			else if (m_capacity <= m_length + 1)
			{
				resize(m_capacity * 2);
			}

			return put_internal(key, value);
		}

		V& operator[](K& key)
		{
			int index = 0;
			if (hasKey_internal(k, index))
			{
				return get(index).m_value;
			}

			return HashKeyValue<K,V>().m_value;
		}

		bool hasKey_internal(K& key, int& index)
		{
			int currentCapacity = m_capacity;
			ZE::UInt32 hashValue = Func(key);
			while (currentCapacity >= 0)
			{
				int initial_index = hashValue % currentCapacity;
				index = initial_index;

				int offset = 1;
				
				while (get(index).m_occupied && get(index).m_hashKey != hashValue && index < currentCapacity)
				{
#if HASH_MAP_PROBING == HASH_MAP_LINEAR_PROBING
					index = (index + 1) % m_capacity;
#elif HASH_MAP_PROBING == HASH_MAP_QUADRATIC_PROBING
					index = (initial_index + (offset * offset)) % currentCapacity;
#endif
					offset++;
				}

				if (get(index).m_hashKey == hashValue)
				{
					return true;
				}

				if (offset >= currentCapacity) // already search whole capacity
				{
					return false;
				}

				currentCapacity /= 2;
			}

			index = -1;

			return false;
		}

		bool hasKey(K& key)
		{
			int index = 0;
			return hasKey_internal(key, index);
		}
	};

	template<class V>
	class HashMap<String, V, HashFunc<String>> : public Array<HashKeyValue<String, V>, true>
	{

	public:

		V& operator[](const char* key)
		{
			int index = 0;
			if (hasKey_internal(key, index))
			{
				return get(index).m_value;
			}

			return HashKeyValue<String,V>().m_value;
		}

		bool put_internal(const char* key, const V& value)
		{
			ZE::UInt32 hashValue = StringFunc::Hash(key, StringFunc::Length(key));
			int initial_index = hashValue % m_capacity;
			int index = initial_index;

			// TODO Check if the index currently occupied
			// TODO Probing until find empty spot
			// Question? How to check if the spot is empty
#if HASH_MAP_PROBING == HASH_MAP_QUADRATIC_PROBING
			int q_offset = 1;
#endif
			while (get(index).m_occupied)
			{
				// Try to reposition the current index for better performance
				HashKeyValue<String, V>& hashKeyValue = get(index);
				hashKeyValue.m_occupied = false;
				m_length--;
				put_internal(hashKeyValue.m_key.c_str(), hashKeyValue.m_value);
				
				if(!get(index).m_occupied) { break; }

#if HASH_MAP_PROBING == HASH_MAP_LINEAR_PROBING
				index = (index + 1) % m_capacity;
#elif HASH_MAP_PROBING == HASH_MAP_QUADRATIC_PROBING
				index = (initial_index + (q_offset * q_offset)) % m_capacity;
				q_offset++;
#endif
			}

			HashKeyValue<String, V>& hashKeyValue = get(index);
			hashKeyValue.m_occupied = true;
			m_length++;
			hashKeyValue.m_hashKey = hashValue;
			hashKeyValue.m_key = key;
			hashKeyValue.m_value = value;

			return true;
		}

		bool put(const char* key, const V& value)
		{
			int index = 0;
			if (hasKey_internal(key, index))
			{
				HashKeyValue<String, V>& hashKeyValue = get(index);
				hashKeyValue.m_value = value;
				return true;
			}

			if (m_capacity == 0)
			{
				reset(1);
			}
			else if (m_capacity <= m_length + 1)
			{
				resize(m_capacity * 2);
			}

			return put_internal(key, value);
		}

		bool hasKey_internal(const char* key, int& index)
		{
			int currentCapacity = m_capacity;
			ZE::UInt32 hashValue = StringFunc::Hash(key, StringFunc::Length(key));
			while (currentCapacity > 0)
			{
				int initial_index = hashValue % currentCapacity;
				index = initial_index;

				int offset = 1;

				while (get(index).m_occupied && get(index).m_hashKey != hashValue && offset < currentCapacity)
				{
#if HASH_MAP_PROBING == HASH_MAP_LINEAR_PROBING
					index = (index + 1) % currentCapacity;
#elif HASH_MAP_PROBING == HASH_MAP_QUADRATIC_PROBING
					index = (initial_index + (offset * offset)) % currentCapacity;
#endif
					offset++;
				}

				if (get(index).m_hashKey == hashValue)
				{
					return true;
				}

				if (offset >= currentCapacity) // already search whole capacity
				{
					index = -1;
					return false;
				}

				currentCapacity /= 2;
			}

			index = -1;

			return false;
		}

		bool hasKey(const char* key)
		{
			int index = 0;
			return hasKey_internal(key, index);
		}
	};


}
#endif // ! __ZE_HASHMAP__

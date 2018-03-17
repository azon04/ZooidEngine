#ifndef __Z_BUFFER_DATA__
#define __Z_BUFFER_DATA__

#include "Utils/PrimitiveTypes.h"

namespace ZE {

enum BufferType {
	UNIFORM_BUFFER,
	VERTEX_BUFFER,
	INDEX_BUFFER,
	COMPUTE_BUFFER
};

class BufferData {

public:
	BufferData(BufferType type) : m_type(type)
	{}

	virtual ~BufferData() {}

	void SetData(void* Data, unsigned int sizePerItem, unsigned int count = 1) {
		m_data = Data;
		m_size = sizePerItem * count;
		m_count = count;
	}

	void* m_data;
	ZE::UInt32 m_count;
	ZE::UInt32 m_size;
	BufferType m_type;
	ZE::Int32 m_bufferLayout;
};

};
#endif // 

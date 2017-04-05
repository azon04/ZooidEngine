#ifndef __Z_GPU_BUFFER__
#define __Z_GPU_BUFFER__

#include "BufferData.h"

#include <vector>

#define MAX_LAYOUT 16
namespace ZE {
enum DataType {
	FLOAT
};

struct BufferLayout {
	int index;
	int sizePerItem;
	DataType dataType;
	int stride;
	int offset;
};

class GPUBufferData {

public:
	GPUBufferData() : m_BBO(0) {}
	virtual ~GPUBufferData() {}

	void FromBufferData(BufferData* _bufferData);
	void SetupLayout(BufferLayout* _layouts, int numberOfLayout);

	void Bind();
	void UnBind();

	BufferType m_bufferType;
	BufferData* m_BufferData;

	// #OPENGL Specific
	unsigned int m_BBO;
	int numberOfLayout;
	std::vector<BufferLayout> m_layouts;

};
};
#endif // __Z_GPU_BUFFER__


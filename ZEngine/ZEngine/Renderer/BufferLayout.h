#ifndef __ZE_BUFFER_LAYOUT__
#define __ZE_BUFFER_LAYOUT__

#include <vector>

#define BUFFER_LAYOUT_V3_C2 0

namespace ZE {

	enum DataType {
		FLOAT
	};

	struct Layout {
		int index;
		int sizePerItem;
		DataType dataType;
		int stride;
		int offset;
	};

	class BufferLayout {
	public:
		std::vector<Layout> m_layouts;

		void BindLayout();

	};

	class BufferLayoutManager {
	public:
		void InitLayout();
		void DestroyLayout();

		std::vector<BufferLayout*> m_bufferLayout;

		BufferLayout* getBufferLayoutByFormat(int format);
	};
}


#endif // __ZE_BUFFER_LAYOUT__

#ifndef __ZE_BUFFER_LAYOUT__
#define __ZE_BUFFER_LAYOUT__

#include <vector>

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

		void InitLayout();
		void DestroyLayout();

		std::vector<BufferLayout*> m_bufferLayout;
	};
}


#endif // __ZE_BUFFER_LAYOUT__

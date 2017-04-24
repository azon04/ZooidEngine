#ifndef __ZE_BUFFER_LAYOUT__
#define __ZE_BUFFER_LAYOUT__

#include <vector>

#define BUFFER_LAYOUT_V3_C3 0
#define BUFFER_LAYOUT_V3_TC2 1
#define BUFFER_LAYOUT_v3 2

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
		static BufferLayoutManager* m_instance;
	public:
		static void Init();
		static BufferLayoutManager* getInstance();
		static void Destroy();

		void InitLayout();
		void DestroyLayout();

		std::vector<BufferLayout*> m_bufferLayout;

		BufferLayout* getBufferLayoutByFormat(int format);
	};
}


#endif // __ZE_BUFFER_LAYOUT__

#ifndef __ZE_BUFFER_LAYOUT__
#define __ZE_BUFFER_LAYOUT__

#include "../Utils/PrimitiveTypes.h"
#include "../Utils/Array.h"


#define BUFFER_LAYOUT_V3_C3 0
#define BUFFER_LAYOUT_V3_TC2 1
#define BUFFER_LAYOUT_V3_C3_TC2 2

namespace ZE {

	enum DataType {
		FLOAT
	};

	struct Layout {
		ZE::Int32 index;
		ZE::UInt32 sizePerItem;
		DataType dataType;
		ZE::UInt32 stride;
		ZE::Int32 offset;
	};

	class BufferLayout {
	public:
		Array<Layout> m_layouts;

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

		Array<BufferLayout*> m_bufferLayout;

		BufferLayout* getBufferLayoutByFormat(int format);
	};
}


#endif // __ZE_BUFFER_LAYOUT__

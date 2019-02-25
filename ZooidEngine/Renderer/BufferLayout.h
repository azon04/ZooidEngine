#ifndef __ZE_BUFFER_LAYOUT__
#define __ZE_BUFFER_LAYOUT__

#include "Utils/PrimitiveTypes.h"
#include "Utils/Array.h"
#include "Enums.h"

#define BUFFER_LAYOUT_V3_C3 0
#define BUFFER_LAYOUT_V3_TC2 1
#define BUFFER_LAYOUT_V3_C3_TC2 2
#define BUFFER_LAYOUT_V3_N3_TC2 3
#define BUFFER_LAYOUT_V3_N3_TC2_SKIN 4
#define BUFFER_LAYOUT_V4 5
#define BUFFER_LAYOUT_V3 6

namespace ZE 
{

	struct Layout
	{
		ZE::Int32 Index;
		ZE::UInt32 SizePerItem;
		EDataType DataType;
		ZE::UInt32 Stride;
		ZE::Int32 Offset;
		bool bInstanceData;
	};

	class BufferLayout 
	{

	public:

		// Get buffer data count
		UInt32 getBufferDataCount() const { return m_bufferDataCount; }

		// Calculate buffer data count
		UInt32 calculateBufferDataCount();

	public:
		Array<Layout> m_layouts;

	protected:
		UInt32 m_bufferDataCount;
	};

	class BufferLayoutManager 
	{
		
	public:

		// Init buffer layout manager
		static void Init();

		// Get instance of BufferLayoutManager
		static BufferLayoutManager* GetInstance();

		// Destroy instance of buffer layout manager
		static void Destroy();

		// Init layout for the manager
		void InitLayout();

		// Destroy all layout for the manager
		void DestroyLayout();

		// get buffer layout by format type
		BufferLayout* getBufferLayoutByFormat(int format);

		// Add new Buffer Layout to the manager: Return index
		UInt32 addNewBufferLayout(BufferLayout* bufferLayout);

		// Create and Add Empty Buffer Layout
		BufferLayout* createAndAddBufferLayout(UInt32& bufferIndex);

	protected:
		// List of the buffer layouts
		Array<BufferLayout*> m_bufferLayout;

	private:
		static BufferLayoutManager* m_instance;
	};
}


#endif // __ZE_BUFFER_LAYOUT__

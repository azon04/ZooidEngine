#ifndef __ZE_DEBUG_RENDERER_H__
#define __ZE_DEBUG_RENDERER_H__

#include "GameObjectModel/Component.h"
#include "Utils/Array.h"
#include "Math/Vector3.h"
#include "Math/Matrix4x4.h"

#define MAX_LINE 2048

namespace ZE
{
	class GameContext;
	class BufferData;
	class IGPUBufferData;
	class IGPUBufferArray;

	struct DebugPointStruct
	{
		Vector3 Point;
		Vector3 Color;
	};

	class DebugRenderer : public Component
	{

		DEFINE_CLASS(DebugRenderer)

	public:

		DebugRenderer(GameContext* gameContext) 
			: Component(gameContext)
		{}

		virtual ~DebugRenderer() {}

		static void Init(GameContext* gameContext);
		static void Destroy();
		static DebugRenderer* GetInstance() { return s_instance; }

		virtual void setupComponent();

		void handleGatherRender(Event* _event);

		void drawMatrixBasis(Matrix4x4& mats);
		void drawLine(const Vector3& p1, const Vector3& p2, const Vector3& color);
	
	private:
		static DebugRenderer* s_instance;
		BufferData* m_lineBufferData;
		IGPUBufferData* m_lineGPUBufferData;
		IGPUBufferArray* m_lineBufferArray;
		DebugPointStruct m_lineBuffers[MAX_LINE * 2];
		Int32 m_currentIndex;
	};
}
#endif

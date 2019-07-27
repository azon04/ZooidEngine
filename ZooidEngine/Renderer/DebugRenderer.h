#ifndef __ZE_DEBUG_RENDERER_H__
#define __ZE_DEBUG_RENDERER_H__

#include "GameObjectModel/Component.h"
#include "Utils/Array.h"
#include "Math/Vector3.h"
#include "Math/Vector2.h"
#include "Math/Matrix4x4.h"

namespace ZE
{
	class GameContext;
	class BufferData;
	class IGPUBufferData;
	class IGPUBufferArray;
	class TextComponent;

	struct DebugPointStruct
	{
		Vector3 Point;
		Vector3 Color;
	};

	class DebugRenderer : public Component
	{

		DEFINE_CLASS(DebugRenderer)

	public:

		DebugRenderer(GameContext* gameContext);

		virtual ~DebugRenderer() {}

		static void Init(GameContext* gameContext);
		static void Destroy();
		static DebugRenderer* GetInstance() { return s_instance; }

		// Draw Matrix Basis (Static call)
		static void DrawMatrixBasis(Matrix4x4& mats);
		
		// Draw Line in 3D world (Static call)
		static void DrawLine(const Vector3& p1, const Vector3& p2, const Vector3& color);

		// Draw Text on Screen (Static call)
		static void DrawTextScreen(const char* text, const Vector2& _position, const Vector3& _color, Float32 _scale = 1.0f);

		// Draw Text in World Space (Static call)
		static void DrawTextWorld(const char* text, Matrix4x4& _transform);

		// Draw Debug Sphere (Static call)
		static void DrawDebugSphere(const Vector3& position, Float32 radius);

		// Draw Debug Box (Static call)
		static void DrawDebugBox(const Vector3& extend, const Vector3& posOffset, Matrix4x4& tranform);

		// Draw Debug Cone (Static call)
		static void DrawDebugCone(const Vector3& top, float distance, float radius, Matrix4x4& transform);

		virtual void setupComponent() override;

		// Handle Gather render event
		void handleGatherRender(Event* _event);

	protected:
		// Draw Matrix Basis
		void drawMatrixBasis(Matrix4x4& mats);
		
		// Draw Line in 3D world
		void drawLine(const Vector3& p1, const Vector3& p2, const Vector3& color);
		
		// Draw Text on Screen
		void drawScreenText(const char* text, const Vector2& _position, const Vector3& _color, Float32 _scale = 1.0f);

		// Draw Text on World
		void drawWorldText(const char* text, Matrix4x4& _transform);

		// Draw Debug Sphere
		void drawDebugSphere(Matrix4x4& _transform);

		// Draw Debug Cube
		void drawDebugCube(Matrix4x4& _transform);

		// Draw Debug Cone
		void drawDebugCone(Matrix4x4& _transform);

		// Setup sphere
		void setupSphere();

		// Setup Cube
		void setupCube();

		// Setup Cone
		void setupCone();

	private:
		static DebugRenderer* s_instance;
		
		BufferData* m_lineBufferData;
		IGPUBufferData* m_lineGPUBufferData;
		IGPUBufferArray* m_lineBufferArray;

		Array<DebugPointStruct> m_lineBuffers;
		Array<TextComponent*> m_textComponents;
		
		Int32 m_currentIndex;
		Int32 m_currentTextIndex;

		IGPUBufferArray* m_sphereGPUBufferArrray;
		Array<Matrix4x4> m_debugSphereTransforms;

		IGPUBufferArray* m_cubeGPUBufferArray;
		Array<Matrix4x4> m_debugCubeTransforms;

		IGPUBufferArray* m_coneGPUBufferArray;
		Array<Matrix4x4> m_debugConeTransforms;
	};
}
#endif

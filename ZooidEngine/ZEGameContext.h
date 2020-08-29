#ifndef __ZE_GAME_CONTEXT__
#define __ZE_GAME_CONTEXT__

#include "Platform/Thread.h"
#include "Utils/Timer.h"
#include "Utils/PrimitiveTypes.h"

#if ZE_RENDER_MULTITHREAD
#define MAX_DRAWLIST_BUFFER 2
#else
#define MAX_DRAWLIST_BUFFER 1
#endif

namespace ZE 
{
	class MaterialManager;
	class MeshManager;
	class RenderZooid;
	class SceneManager;
	class SceneComponent;
	class BufferManager;
	class ShaderManager;
	class DrawList;
	class EventDispatcher;
	class TextureManager;
	class InputManager;
	class IRenderer;
	class CameraManager;
	class PhysicsZooid;
	class IPhysics;
	class DebugRenderer;

	class GameContext {
	public:
		GameContext() {}
		virtual ~GameContext() {}

		RenderZooid* m_renderZooid;
		IRenderer* m_renderer;

		// Physics
		PhysicsZooid* m_physicsZooid;
		IPhysics* m_physics;

		BufferManager* m_bufferManager;
		ShaderManager* m_shaderManager;
		DrawList* m_drawLists[MAX_DRAWLIST_BUFFER];
		CameraManager* m_cameraManager;
		EventDispatcher* m_mainEventDispatcher;
		SceneComponent* m_rootComponent;
		TextureManager* m_textureManager;
		InputManager* m_inputManager;

		MaterialManager* m_materialManager;
		MeshManager* m_meshManager;

		SceneManager* m_sceneManager;
		DebugRenderer* m_debugRenderer;

		Thread* m_drawThread;

		Timer m_mainTimer;
		Timer m_globalTimer;
		Timer m_renderThreadTimer;

		Int32 m_currentGameDrawlist = 0;
		Int32 m_currentRenderDrawlist = 0;

		FORCEINLINE RenderZooid* getRenderZooid() const { return m_renderZooid; }
		FORCEINLINE IRenderer* getRenderer() const { return m_renderer; }

		FORCEINLINE BufferManager* getBufferManager() const { return m_bufferManager; }
		FORCEINLINE ShaderManager* getShaderManager() const { return m_shaderManager; }
		FORCEINLINE DrawList* getDrawList(int index) const { return m_drawLists[index]; }
		FORCEINLINE DrawList* getGameDrawList() const { return m_drawLists[m_currentGameDrawlist]; } // Draw list to write into
		FORCEINLINE DrawList* getRenderDrawList() const { return m_drawLists[m_currentRenderDrawlist]; } // Draw list to be read by rendering job
		FORCEINLINE CameraManager* getCameraManager() const { return m_cameraManager; }
		FORCEINLINE SceneComponent* getRootComponent() const { return m_rootComponent; }
		FORCEINLINE EventDispatcher* getEventDispatcher() const { return m_mainEventDispatcher; }
		FORCEINLINE TextureManager* getTextureManager() const { return m_textureManager; }
		FORCEINLINE InputManager* getInputManager() const { return m_inputManager; };
		FORCEINLINE MaterialManager* getMaterialManager() const { return m_materialManager; }
		FORCEINLINE MeshManager* getMeshManager() const { return m_meshManager; }
		FORCEINLINE SceneManager* getSceneManager() const { return m_sceneManager; }
		FORCEINLINE DebugRenderer* getDebugRenderer() const { return m_debugRenderer; }

		// Physics
		FORCEINLINE PhysicsZooid* getPhysicsZooid() const { return m_physicsZooid; }
		FORCEINLINE IPhysics* getPhysics() const { return m_physics; }
	};

	extern GameContext* gGameContext;

	extern Int32 gRenderWidth;
	extern Int32 gRenderHeight;

	GameContext* GetGameContext();
}
#endif // __ZE_GAME_CONTEXT__

#ifndef __ZE_GAME_CONTEXT__
#define __ZE_GAME_CONTEXT__

#include "Platform/Thread.h"
#include "Utils/Timer.h"

namespace ZE {
	
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

	class GameContext {
	public:
		GameContext() {}
		virtual ~GameContext() {}

		RenderZooid* m_renderZooid;
		IRenderer* m_renderer;
		BufferManager* m_bufferManager;
		ShaderManager* m_shaderManager;
		DrawList* m_drawList;
		CameraManager* m_cameraManager;
		EventDispatcher* m_mainEventDispatcher;
		SceneComponent* m_rootComponent;
		TextureManager* m_textureManager;
		InputManager* m_inputManager;

		MaterialManager* m_materialManager;
		MeshManager* m_meshManager;

		SceneManager* m_sceneManager;
		
		Thread* m_drawThread;

		Timer m_mainTimer;
		Timer m_globalTimer;
		Timer m_renderThreadTimer;

		FORCEINLINE RenderZooid* getRenderZooid() const { return m_renderZooid; }
		FORCEINLINE IRenderer* getRenderer() const { return m_renderer; }
		FORCEINLINE BufferManager* getBufferManager() const { return m_bufferManager; }
		FORCEINLINE ShaderManager* getShaderManager() const { return m_shaderManager; }
		FORCEINLINE DrawList* getDrawList() const { return m_drawList; }
		FORCEINLINE CameraManager* getCameraManager() const { return m_cameraManager; }
		FORCEINLINE SceneComponent* getRootComponent() const { return m_rootComponent; }
		FORCEINLINE EventDispatcher* getEventDispatcher() const { return m_mainEventDispatcher; }
		FORCEINLINE TextureManager* getTextureManager() const { return m_textureManager; }
		FORCEINLINE InputManager* getInputManager() const { return m_inputManager; };
		FORCEINLINE MaterialManager* getMaterialManager() const { return m_materialManager; }
		FORCEINLINE MeshManager* getMeshManager() const { return m_meshManager; }
		FORCEINLINE SceneManager* getSceneManager() const { return m_sceneManager; }

	};
}
#endif // __ZE_GAME_CONTEXT__

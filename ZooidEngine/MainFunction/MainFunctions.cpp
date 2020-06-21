#include "MainFunctions.h"

#include "Common/GlobalRegistry.h"

#include "ZEngine.h"

#include "Application/Application.h"

#include "Renderer/IGPUTexture.h"
#include "Memory/Handle.h"
#include "Events/Events.h"
#include "FileSystem/DirectoryHelper.h"
#include "Scene/SceneManager.h"
#include "Scene/CameraManager.h"
#include "Scene/CameraComponent.h"

#include "Logging/Log.h"

#if ZE_RENDER_OPENGL
#include "Renderer/GL/GLRenderZooid.h"
#endif

#if Z_PHYSICS_PHYSX
#include "Physics/PhysX/PhysXZooid.h"
#endif

#include "Physics/Physics.h"

#include "Platform/Thread.h"

#include "ResourceManagers/SkeletonManager.h"
#include "ResourceManagers/AnimationManager.h"
#include "ResourceManagers/FontManager.h"

#include "Renderer/DebugRenderer.h"
#include "Renderer/IGPUFrameBuffer.h"
#include "Renderer/IGPUStates.h"
#include "Renderer/ShaderData.h"

#include "scene/Light/LightComponent.h"

#include "UI/UIManager.h"

#include "Math/MathOps.h"

#include "SceneRenderer/SceneRenderer.h"
#include "SceneRenderer/RenderGatherer.h"
#include "SceneRenderer/ShadowRenderer.h"
#include "SceneRenderer/TextRenderer.h"
#include "SceneRenderer/SkyboxRenderer.h"
#include "SceneRenderer/ForwardRenderPass.h"

#include "Utils/DebugOptions.h"

#define DEFERRED_RENDERING 1

#if DEFERRED_RENDERING
#include "SceneRenderer/RenderGraph/BaseDeferredRenderGraph.h"
#else
#include "SceneRenderer/RenderGraph/BaseForwardRenderGraph.h"
#endif

// TODO for NVIDIA Optimus :  This enable the program to use NVIDIA instead of integrated Intel graphics
#if WIN32 || WIN64
extern "C"
{
#include <windows.h>
	_declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
}
#endif

ZE::Float32 g_gameThreadTime;
ZE::Float32 g_gpuDrawTime;

namespace ZE 
{
	ConditionVariable g_drawThreadVariable;
	Mutex g_drawMutex;
	bool g_drawReady = false;

	void MainSetup(GameContext* _gameContext)
	{
		gGameContext = _gameContext;

		ZEINFO("Zooid Engine Main Setup");
		ZEINFO("=======================");

		ZASSERT(ZE::MAIN_THREAD_ID == ZE::getThreadId(), "Not in Main Thread");

		// Construct Memory
		ZEINFO("Initializing Memory Manager...");
		MemoryManager::Construct();

		// Register all classes
		ZEINFO("Registering Classes...");
		GlobalRegistry::Register();

		// Creating rendering Zooid

		{
#if ZE_RENDER_OPENGL
			ZEINFO("Initializing GL Rendering...");
			Handle renderZooidHandle("Render Zooid", sizeof(GLRenderZooid));
			_gameContext->m_renderZooid = new(renderZooidHandle) GLRenderZooid(_gameContext);
#endif
			_gameContext->m_renderZooid->Init();
			_gameContext->m_renderer = _gameContext->m_renderZooid->GetRenderer();
		}

		ZEINFO("Initializing Shaders...");
		ShaderManager::Init(_gameContext);
		_gameContext->m_shaderManager = ShaderManager::GetInstance();

		ZEINFO("Initializing Buffer Manager...");
		BufferManager::Init(_gameContext);
		_gameContext->m_bufferManager = BufferManager::getInstance();
		
		ZEINFO("Initializing Texture Manager...");
		TextureManager::Init(_gameContext);
		_gameContext->m_textureManager = TextureManager::GetInstance();

		ZEINFO("Initializing Material Manager...");
		MaterialManager::Init();
		_gameContext->m_materialManager = MaterialManager::GetInstance();

		ZEINFO("Initializing Mesh Manager...");
		MeshManager::Init();
		_gameContext->m_meshManager = MeshManager::GetInstance();

		{
			ZEINFO("Initializing DrawList...");
			Handle handle("DrawList", sizeof(DrawList));
			_gameContext->m_drawList = new(handle) DrawList;
			_gameContext->m_drawList->Setup();
		}

		// Create Main Event Dispatcher
		{
			ZEINFO("Initializing Event Dispatcher...");
			Handle handle("EventDispatcher", sizeof(EventDispatcher));
			_gameContext->m_mainEventDispatcher = new(handle) EventDispatcher(_gameContext);
			_gameContext->m_mainEventDispatcher->setupComponent();
		}

		// Create Root Component and add is as child of Event Dispatcher
		{
			ZEINFO("Initializing Root Component...");
			Handle handle("RootComponent", sizeof(SceneComponent));
			_gameContext->m_rootComponent = new (handle) SceneComponent(_gameContext);
			_gameContext->getEventDispatcher()->addChild(_gameContext->m_rootComponent);
			_gameContext->m_rootComponent->setupComponent();
			_gameContext->m_rootComponent->setObjectName("Root Component");
		}

		// Init Font Manager
		{
			ZEINFO("Initializing Font Manager...");
			FontManager::Init(_gameContext);
		}

		// Init Debug Renderer
		{
			ZEINFO("Initializing Debug Renderer...");
			DebugRenderer::Init(_gameContext);
			_gameContext->m_debugRenderer = DebugRenderer::GetInstance();
			_gameContext->m_rootComponent->addChild(_gameContext->m_debugRenderer);
		}

		// Create SceneManager
		{
			SceneManager::Init(_gameContext);
			_gameContext->m_sceneManager = SceneManager::GetInstance();
		}

		// Create Input Manager
		{
			ZEINFO("Initializing Input Manager...");
			Handle handle("InputManager", sizeof(InputManager));
			_gameContext->m_inputManager = new (handle) InputManager(_gameContext);
			_gameContext->m_inputManager->setObjectName("InputManager");
			_gameContext->m_inputManager->setupComponent();
		}

		// Create Physics
		{
#if Z_PHYSICS_PHYSX
			ZEINFO("Initializing Physics...");
			Handle hPhysXZooidHandle("PhysX Zooid", sizeof(ZE::PhysXZooid));
			_gameContext->m_physicsZooid = new (hPhysXZooidHandle) ZE::PhysXZooid(_gameContext);
#endif
			_gameContext->m_physicsZooid->Init();
			_gameContext->m_physics = _gameContext->m_physicsZooid->GetPhysics();
		}

		ZEINFO("Initializing Skeleton Manager...");
		SkeletonManager::Init(_gameContext);

		ZEINFO("Initializing Animation Resource Manager...");
		AnimationManager::Init(_gameContext);

		ZEINFO("Initializing Camera Manager...");
		CameraManager::Init(_gameContext);
		_gameContext->m_cameraManager = CameraManager::GetInstance();
		
		ZEINFO("Initializing Zooid UI...");
		UIManager::Init(_gameContext);

		// Application setup
		Application::GetApplication()->Setup(_gameContext);

#if DEFERRED_RENDERING
		BaseDeferredRenderGraph::GetInstance()->init(_gameContext);
#else
		BaseForwardRenderGraph::GetInstance()->init(_gameContext);
#endif

		_gameContext->m_mainTimer.Reset();

#if ZE_RENDER_MULTITHREAD
		g_drawReady = false;
		_gameContext->m_drawThread = new Thread(MainDrawJob, _gameContext);
#endif

	}

	void MainClean(GameContext* _gameContext)
	{
#if ZE_RENDER_MULTITHREAD
		if (_gameContext->m_drawThread)
		{
			_gameContext->m_drawThread->join();
		}
#endif

#if TEST_DEFERRED_RENDERING
		BaseDeferredRenderGraph::GetInstance()->release(_gameContext);
#endif

		// Application Clean
		Application::GetApplication()->Clean(_gameContext);

		IGPUState::ClearGPUStates();
		UIManager::Destroy();

		CameraManager::Destroy();
		
		_gameContext->m_physicsZooid->Destroy();
		
		DebugRenderer::Destroy();
		FontManager::Destroy();
		AnimationManager::Destroy();
		SkeletonManager::Destroy();
		
		BufferManager::Destroy();
		ShaderManager::Destroy();
		TextureManager::Destroy();

		_gameContext->m_renderZooid->Destroy();

		MemoryManager::Deconstruct();
	}

	void MainThreadJob(GameContext* _gameContext)
	{
		//ZELOG(LOG_ENGINE, Log , "Test %d", 1);
		double deltaTime = _gameContext->m_mainTimer.ResetAndGetDeltaMS();
		float deltaSeconds = static_cast<Float32>(deltaTime * 0.001f);

		// UI Begin
		ZE::UI::BeginFrame();

		// Handle Event_Update
		{
			ZE::Handle handleUpdate("EventUpdate", sizeof(ZE::Event_UPDATE));
			ZE::Event_UPDATE* eventUpdate = new(handleUpdate) ZE::Event_UPDATE();
			eventUpdate->m_deltaMilliseconds = deltaTime;
			eventUpdate->m_deltaSeconds = deltaSeconds;
			_gameContext->getEventDispatcher()->handleEvent(eventUpdate);
			_gameContext->getInputManager()->handleEvent(eventUpdate);
			handleUpdate.release();
		}

		for (int i = 0; i < _gameContext->getEventDispatcher()->getEvents(ZE::EVENT_INPUT).length(); i++)
		{
			ZE::Handle handle = _gameContext->getEventDispatcher()->getEvents(ZE::EVENT_INPUT)[i];
			if (handle.isValid())
			{
				_gameContext->getEventDispatcher()->handleEvent(handle.getObject<ZE::Event>());
			}
			handle.release();
		}
		_gameContext->getEventDispatcher()->clearEvents(ZE::EVENT_INPUT);

		// Update Physics
		if (_gameContext->getPhysics())
		{
			_gameContext->getPhysics()->PreUpdate();
			_gameContext->getPhysics()->Update(deltaSeconds);
			_gameContext->getPhysics()->PostUpdate();
			_gameContext->getPhysics()->DrawDebug();
		}

		// Application Clean
		Application::GetApplication()->Tick(_gameContext, deltaTime);

#if ZE_RENDER_MULTITHREAD
		while (g_drawReady) 
		{
			ZE::ThreadSleep(0);
		}
#endif

		// Draw Base Lines
		//DebugRenderer::DrawMatrixBasis(Matrix4x4());
		//DebugRenderer::DrawTextWorld("Zooid Engine", Matrix4x4());
		//DebugRenderer::DrawTextScreen("Zooid Engine", Vector2(10, 10), Vector3(1.0f), 0.5f);

		// Debug Options Menu
		/*if (ZE::UI::BeginPanel("Debug Options", UIRect(UIVector2{ gGameContext->getRenderer()->GetWidth() - 300,10 }, UIVector2{ 250, 100 }), true))
		{
			gDebugOptions.DebugDrawOptions.bDrawCullShapes = ZE::UI::DoCheckBox("DebugDraw:CullShape", gDebugOptions.DebugDrawOptions.bDrawCullShapes);
			gDebugOptions.DebugDrawOptions.bDrawPhysicsShapes = ZE::UI::DoCheckBox("DebugDraw:PhysicsShape", gDebugOptions.DebugDrawOptions.bDrawPhysicsShapes);
			ZE::UI::EndPanel();
		}*/

		DrawList* drawList = _gameContext->getDrawList();

		// Set up data for this frame
		{
			// Set ViewFustrum before gathering the render target
			CameraComponent* currentCamera = CameraManager::GetInstance()->getCurrentCamera();

			// Compute View Matrix and View Frustum
			drawList->m_viewFustrum.setCameraVars(currentCamera->getWorldPosition(), currentCamera->getForwardVector() * -1.0f, currentCamera->getUpVector(), currentCamera->getRightVector());
			currentCamera->getViewMatrix(drawList->m_viewMat);
			drawList->m_viewFustrum.constructFromMVPMatrix(drawList->m_viewMat * drawList->m_projectionMat);

			drawList->m_objectsBounding.m_max = Vector3(-99999.9f);
			drawList->m_objectsBounding.m_min = Vector3(99999.9f);

			_gameContext->getDrawList()->m_lightData.NumLight = 0;
			_gameContext->getDrawList()->m_lightData.NumCascade = 0;
		}

		// Handle Event_GATHER_BOUND
		{
			Handle handleGatherBound("EventGatherBound", sizeof(Event_GATHER_BOUND));
			Event_GATHER_BOUND* pEvent = new(handleGatherBound) Event_GATHER_BOUND;
			_gameContext->getEventDispatcher()->handleEvent(pEvent);
			handleGatherBound.release();
		}

		// Handle Event_GATHER_RENDER
		{
			Handle handleGatherRender("EventGatherRender", sizeof(Event_GATHER_RENDER));
			Event_GATHER_RENDER* pEvent = new(handleGatherRender) Event_GATHER_RENDER();
			_gameContext->getEventDispatcher()->handleEvent(pEvent);
			handleGatherRender.release();
		}

		// Handle Event_GATHER_LIGHT
		{
			Handle handleGatherLight("EventGatherLight", sizeof(ZE::Event_GATHER_LIGHT));
			Event_GATHER_LIGHT* eventGatherLight = new(handleGatherLight) Event_GATHER_LIGHT();
			_gameContext->getEventDispatcher()->handleEvent(eventGatherLight);
			handleGatherLight.release();
		}

		// Handle Event_GATHER_SHADOW_LIST
		{
			Handle handleGatherShadowList("EventGatherShadowList", sizeof(Event_GATHER_SHADOW_LIST));
			Event_GATHER_SHADOW_LIST* eventGatherShadow = new(handleGatherShadowList) Event_GATHER_SHADOW_LIST;
			for (UInt32 i = 0; i < drawList->m_lightShadowSize; i++)
			{
				LightShadowMapData& shadowMapData = drawList->m_lightShadowMapData[i];
				shadowMapData.meshRenderGatherer.reset();
				shadowMapData.skinMeshRenderGatherer.reset();
				if (shadowMapData.dynamicShadowFrameBuffer)
				{
					eventGatherShadow->m_shadowDataIndex = i;
					_gameContext->getEventDispatcher()->handleEvent(eventGatherShadow);
				}
			}
			handleGatherShadowList.release();
		}

		// Write on Total Delta time
		g_gameThreadTime = MathOps::FLerp(g_gameThreadTime, (Float32)deltaTime, 0.01f);

		/*if (ZE::UI::BeginPanel("Performance", UIRect(UIVector2{ 10,10 }, UIVector2{ 250, 150 }), true))
		{
			char buffer[256];

			StringFunc::PrintToString(buffer, 256, "Global Time: %.2fms", g_gameThreadTime);
			ZE::UI::DoText(buffer);

			StringFunc::PrintToString(buffer, 256, "GPU Draw Time: %.2fms", g_gpuDrawTime);
			ZE::UI::DoText(buffer);

			StringFunc::PrintToString(buffer, 256, "FPS: %.2f", 1000.0f / g_gameThreadTime);
			ZE::UI::DoText(buffer);

			DrawList* drawList = _gameContext->getDrawList();
			StringFunc::PrintToString(buffer, 256, "Num Meshes To Draw: %d", drawList->m_meshRenderGatherer.getRenderCount() + drawList->m_skinMeshRenderGatherer.getRenderCount());
			ZE::UI::DoText(buffer);

			ZE::UI::EndPanel();
		}*/

		// UI End Frame
		ZE::UI::EndFrame();

#if ZE_RENDER_MULTITHREAD
		UniqueLock lck(g_drawMutex);
		g_drawReady = true;
		//ZELOG(LOG_ENGINE, Log, "gDrawReady in Main2 %d", g_drawReady);
		g_drawThreadVariable.notify_all();

		// Oddly needed. This will let the main thread know that the window is still active
		_gameContext->getRenderer()->PollEvent();
#else
		_gameContext->getRenderer()->PollEvent();
		DrawJob(_gameContext);
#endif
	}

	void MainDrawJob(GameContext* _gameContext)
	{
		UniqueLock lck(g_drawMutex);

		_gameContext->m_renderThreadTimer.Reset();

		while (!g_drawReady)
		{
			g_drawThreadVariable.wait(lck);
		}

		while (!_gameContext->getRenderer()->IsClose())
		{
			DrawJob(_gameContext);
			
			g_drawReady = false;
			//ZELOG(LOG_ENGINE, Log, "gDrawReady in Draw %d", g_drawReady);
			while (!g_drawReady) g_drawThreadVariable.wait(lck);
		}
	}

	void DrawJob(GameContext* _gameContext)
	{
		double deltaTime = _gameContext->m_renderThreadTimer.ResetAndGetDeltaMS();

		IRenderer* renderer = _gameContext->getRenderer();
		DrawList* drawList = _gameContext->getDrawList();

		// Main Renderer BeginFrame
		renderer->BeginFrame();

		ScopedRenderThreadOwnership renderLock(renderer);

		BufferManager::getInstance()->setupForNextFrame();

		ShadowDepthRenderer::Reset();

		renderer->BeginRender();

		renderer->ClearScreen();
		
#if DEFERRED_RENDERING
		BaseDeferredRenderGraph::GetInstance()->begin(_gameContext);
		BaseDeferredRenderGraph::GetInstance()->execute(_gameContext);
		BaseDeferredRenderGraph::GetInstance()->end(_gameContext);
#else
		BaseForwardRenderGraph::GetInstance()->begin(_gameContext);
		BaseForwardRenderGraph::GetInstance()->execute(_gameContext);
		BaseForwardRenderGraph::GetInstance()->end(_gameContext);
#endif

		// Inject UI Rendering
		{
			ScopeRenderDebug scopeRenderDebug(renderer, "ZooidUI");
			ZE::UI::ProcessDrawList();
		}

		renderer->EndRender();

		// Main Renderer End Frame
		renderer->EndFrame();

		drawList->Reset();

		deltaTime = _gameContext->m_renderThreadTimer.ResetAndGetDeltaMS();
		g_gpuDrawTime = MathOps::FLerp(g_gpuDrawTime, (Float32)deltaTime, 0.01f);
	}

}
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

#include "Physics/PhysicsZooid.h"
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

#include "Profiler/ProfilerZooid.h"

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
			ZEINFO("Initializing Rendering...");
			_gameContext->m_renderZooid = RenderZooid::GetRenderZooid(_gameContext);
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
			for (int i = 0; i < MAX_DRAWLIST_BUFFER; i++)
			{
				Handle handle("DrawList", sizeof(DrawList));
				_gameContext->m_drawLists[i] = new(handle) DrawList;
				_gameContext->m_drawLists[i]->Setup();
			}
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
			ZEINFO("Initializing Physics...");
			_gameContext->m_physicsZooid = PhysicsZooid::GetPhysicsZood(_gameContext);
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
		_gameContext->m_drawThread = new Thread(MainDrawJob, _gameContext);
#endif

	}

	void MainClean(GameContext* _gameContext)
	{
#if ZE_RENDER_MULTITHREAD
		if (_gameContext->m_drawThread)
		{
			g_drawThreadVariable.notify_one();
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
			ZE_PROFILER_SECTION("Event_Update");
			ZE::Handle handleUpdate("EventUpdate", sizeof(ZE::Event_UPDATE));
			ZE::Event_UPDATE* eventUpdate = new(handleUpdate) ZE::Event_UPDATE();
			eventUpdate->m_deltaMilliseconds = deltaTime;
			eventUpdate->m_deltaSeconds = deltaSeconds;
			_gameContext->getEventDispatcher()->handleEvent(eventUpdate);
			_gameContext->getInputManager()->handleEvent(eventUpdate);
			handleUpdate.release();
		}

		{
			ZE_PROFILER_SECTION("Event_Input");
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
		}

		// Update Physics
		if (_gameContext->getPhysics())
		{
			ZE_PROFILER_SECTION("Event_Physics");
			_gameContext->getPhysics()->PreUpdate();
			_gameContext->getPhysics()->Update(deltaSeconds);
			_gameContext->getPhysics()->PostUpdate();
			_gameContext->getPhysics()->DrawDebug();
		}

		// Application Tick
		{
			ZE_PROFILER_SECTION("ApplicationTick");
			Application::GetApplication()->Tick(_gameContext, deltaTime);
		}

		static Timer lockTimer;
#if ZE_RENDER_MULTITHREAD
		{
		LockGuard guard(_gameContext->getGameDrawList()->m_mutex);
#endif

		// Debug Options Menu
		/*if (ZE::UI::BeginPanel("Debug Options", UIRect(UIVector2{ gGameContext->getRenderer()->GetWidth() - 300,10 }, UIVector2{ 250, 100 }), true))
		{
			gDebugOptions.DebugDrawOptions.bDrawCullShapes = ZE::UI::DoCheckBox("DebugDraw:CullShape", gDebugOptions.DebugDrawOptions.bDrawCullShapes);
			gDebugOptions.DebugDrawOptions.bDrawPhysicsShapes = ZE::UI::DoCheckBox("DebugDraw:PhysicsShape", gDebugOptions.DebugDrawOptions.bDrawPhysicsShapes);
			ZE::UI::EndPanel();
		}*/

		DrawList* drawList = _gameContext->getGameDrawList();

		// Set up data for this frame
		{
			ZE_PROFILER_SECTION("UpdateFrameData");
			// Set ViewFustrum before gathering the render target
			CameraComponent* currentCamera = CameraManager::GetInstance()->getCurrentCamera();

			// Get Projection Matrix
			currentCamera->getProjectionMat(drawList->m_projectionMat);

			// Compute View Matrix and View Frustum
			drawList->m_viewFustrum.setProjectionVars(_gameContext->getRenderer()->GetWidth() / _gameContext->getRenderer()->GetHeight(), 45.0f, currentCamera->m_near, currentCamera->m_far);
			// #TODO set ViewFustrum for Orthographic Projection
			drawList->m_viewFustrum.setCameraVars(currentCamera->getWorldPosition(), currentCamera->getForwardVector() * -1.0f, currentCamera->getUpVector(), currentCamera->getRightVector());
			currentCamera->getViewMatrix(drawList->m_viewMat);
			drawList->m_viewFustrum.constructFromMVPMatrix(drawList->m_viewMat * drawList->m_projectionMat);

			drawList->m_objectsBounding.m_max = Vector3(-99999.9f);
			drawList->m_objectsBounding.m_min = Vector3(99999.9f);

			drawList->m_lightData.NumLight = 0;
			drawList->m_lightData.NumCascade = 0;
		}

		// Handle Event_GATHER_BOUND
		{
			ZE_PROFILER_SECTION("Event_GATHER_BOUND");
			Handle handleGatherBound("EventGatherBound", sizeof(Event_GATHER_BOUND));
			Event_GATHER_BOUND* pEvent = new(handleGatherBound) Event_GATHER_BOUND;
			_gameContext->getEventDispatcher()->handleEvent(pEvent);
			handleGatherBound.release();
		}

		// Handle Event_GATHER_RENDER
		{
			ZE_PROFILER_SECTION("Event_GATHER_RENDER");
			Handle handleGatherRender("EventGatherRender", sizeof(Event_GATHER_RENDER));
			Event_GATHER_RENDER* pEvent = new(handleGatherRender) Event_GATHER_RENDER();
			_gameContext->getEventDispatcher()->handleEvent(pEvent);
			handleGatherRender.release();
		}

		// Handle Event_GATHER_LIGHT
		{
			ZE_PROFILER_SECTION("Event_GATHER_LIGHT");
			Handle handleGatherLight("EventGatherLight", sizeof(ZE::Event_GATHER_LIGHT));
			Event_GATHER_LIGHT* eventGatherLight = new(handleGatherLight) Event_GATHER_LIGHT();
			_gameContext->getEventDispatcher()->handleEvent(eventGatherLight);
			handleGatherLight.release();
		}

		// Handle Event_GATHER_SHADOW_LIST
		{
			ZE_PROFILER_SECTION("Event_GATHER_SHADOW_LIST");
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

		if (gDebugOptions.bShowFPSStats)
		{
			static char buffer[256];
			static Vector3 RedColor(1.0f, 0.0f, 0.0f);
			static Vector3 GreenColor(0.0f, 1.0f, 0.0f);
			static Vector3 YellowColor(1.0f, 1.0f, 0.0f);
			static Float32 targetTime = 1.0f / 60.0f;

			Float32 fps = 1000.0f / g_gameThreadTime;
			StringFunc::PrintToString(buffer, 256, "FPS %.2f", fps);
			DebugRenderer::DrawTextScreen(buffer, Vector2(10.0f, gRenderHeight - 50.0f), (fps < 30.0f) ? RedColor : (fps < 59.0f) ? YellowColor : GreenColor);

			StringFunc::PrintToString(buffer, 256, "Game: %.2fms", g_gameThreadTime);
			DebugRenderer::DrawTextScreen(buffer, Vector2(10.0f, gRenderHeight - 80.0f), (g_gameThreadTime < 0.5 * targetTime) ? RedColor : (g_gameThreadTime < targetTime) ? YellowColor : GreenColor);

			StringFunc::PrintToString(buffer, 256, "GPU: %.2fms", g_gpuDrawTime);
			DebugRenderer::DrawTextScreen(buffer, Vector2(10.0f, gRenderHeight - 100.0f), (g_gpuDrawTime < 0.5 * targetTime) ? RedColor : (g_gpuDrawTime < targetTime) ? YellowColor : GreenColor);

		}

		// UI End Frame
		ZE::UI::EndFrame();

#if ZE_RENDER_MULTITHREAD
		_gameContext->getGameDrawList()->m_bReady = true;
		_gameContext->m_currentGameDrawlist = (_gameContext->m_currentGameDrawlist + 1) % MAX_DRAWLIST_BUFFER;

		g_drawThreadVariable.notify_one();
		}
		// Oddly needed. This will let the main thread know that the window is still active
		
		_gameContext->getRenderer()->PollEvent();
#else
		_gameContext->getRenderer()->PollEvent();
		DrawJob(_gameContext);
#endif
	}

	void MainDrawJob(GameContext* _gameContext)
	{
		ZE_PROFILER_THREAD("Render Worker");

		_gameContext->m_renderThreadTimer.Reset();

		while (!_gameContext->getRenderer()->IsClose())
		{
			DrawList* drawList = _gameContext->getRenderDrawList();
			UniqueLock lck(drawList->m_mutex);
			
			g_drawThreadVariable.wait(lck, [] { return gGameContext->getRenderDrawList()->m_bReady || gGameContext->getRenderer()->IsClose(); });

			DrawJob(_gameContext);
			drawList->m_bReady = false;
			_gameContext->m_currentRenderDrawlist = (_gameContext->m_currentRenderDrawlist + 1) % MAX_DRAWLIST_BUFFER;
		}
	}

	void DrawJob(GameContext* _gameContext)
	{
		double deltaTime = _gameContext->m_renderThreadTimer.ResetAndGetDeltaMS();

		IRenderer* renderer = _gameContext->getRenderer();
		DrawList* drawList = _gameContext->getRenderDrawList();

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
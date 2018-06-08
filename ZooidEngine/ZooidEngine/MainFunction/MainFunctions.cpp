#include "MainFunctions.h"

#include "Common/GlobalRegistry.h"

#include "ZEngine.h"

#include "Renderer/IGPUTexture.h"
#include "Memory/Handle.h"
#include "Events/Events.h"
#include "FileSystem/DirectoryHelper.h"
#include "Scene/SceneManager.h"
#include "Scene/CameraManager.h"
#include "Scene/CameraComponent.h"

#include "Logging/Log.h"

#if Z_RENDER_OPENGL
#include "Renderer/GL/GLRenderZooid.h"
#endif

#if Z_PHYSICS_PHYSX
#include "Physics/PhysX/PhysXZooid.h"
#endif

#include "Physics/Physics.h"

#include "Platform/Thread.h"

#include "ResourceManagers/SkeletonManager.h"

namespace ZE {

	ConditionVariable g_drawThreadVariable;
	Mutex g_drawMutex;
	bool g_drawReady = false;

	void MainSetup(GameContext* _gameContext)
	{
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
#if Z_RENDER_OPENGL
			ZEINFO("Initializing GL Rendering...");
			Handle renderZooidHandle("Render Zooid", sizeof(GLRenderZooid));
			_gameContext->m_renderZooid = new(renderZooidHandle) GLRenderZooid(_gameContext);
#endif
			_gameContext->m_renderZooid->Init();
			_gameContext->m_renderer = _gameContext->m_renderZooid->GetRenderer();
		}

		ZEINFO("Initializing Shaders...");
		ShaderManager::Init(_gameContext);
		_gameContext->m_shaderManager = ShaderManager::getInstance();

		ZEINFO("Initializing Buffer Manager...");
		BufferManager::Init(_gameContext);
		_gameContext->m_bufferManager = BufferManager::getInstance();
		
		ZEINFO("Initializing Texture Manager...");
		TextureManager::Init(_gameContext);
		_gameContext->m_textureManager = TextureManager::getInstance();

		ZEINFO("Initializing Material Manager...");
		MaterialManager::Init();
		_gameContext->m_materialManager = MaterialManager::getInstance();

		ZEINFO("Initializing Mesh Manager...");
		MeshManager::Init();
		_gameContext->m_meshManager = MeshManager::getInstance();

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
			_gameContext->m_rootComponent->setObjectName("Root Component");
			_gameContext->m_rootComponent->setupComponent();
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

		ZEINFO("Initializing Camera Manager...");
		CameraManager::Init(_gameContext);
		_gameContext->m_cameraManager = CameraManager::GetInstance();
		
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

		CameraManager::Destroy();
		
		_gameContext->m_physicsZooid->Destroy();
		
		BufferManager::Destroy();
		ShaderManager::Destroy();
		TextureManager::Destroy();
		SkeletonManager::Destroy();

		_gameContext->m_renderZooid->Destroy();

		MemoryManager::Deconstruct();
	}

	void MainThreadJob(GameContext* _gameContext)
	{
		double deltaTime = _gameContext->m_mainTimer.ResetAndGetDeltaMS();

		ZELOG(LOG_GAME, Log, "Delta Time : %.2f ms", deltaTime);

		// Handle Event_Update
		{
			ZE::Handle handleUpdate("EventUpdate", sizeof(ZE::Event_UPDATE));
			ZE::Event_UPDATE* eventUpdate = new(handleUpdate) ZE::Event_UPDATE();
			eventUpdate->m_deltaTime = deltaTime;
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
			_gameContext->getPhysics()->Update(deltaTime);
			_gameContext->getPhysics()->PostUpdate();
		}

#if ZE_RENDER_MULTITHREAD
		while (g_drawReady) {}
#endif

		// Draw Base Lines
		{
			ZE::ShaderAction& shaderAction = _gameContext->getDrawList()->getNextShaderAction();
			ZE::IShaderChain* shader = ZE::ShaderManager::getInstance()->getShaderChain(2);

			shaderAction.SetShaderAndBuffer(shader, ZE::BufferManager::getInstance()->m_GPUBufferArrays[2]);
			shaderAction.SetShaderMatVar("modelMat", Matrix4x4());
			shaderAction.SetConstantsBlockBuffer("shader_data", _gameContext->getDrawList()->m_mainConstantBuffer);
		}

		// Handle Event_GATHER_RENDER
		{
			Handle handleGatherRender("EventGatherRender", sizeof(Event_GATHER_RENDER));
			Event_GATHER_RENDER* pEvent = new(handleGatherRender) Event_GATHER_RENDER();
			_gameContext->getEventDispatcher()->handleEvent(pEvent);
			handleGatherRender.release();
		}

		// Handle Event_GATHER_LIGHT
		_gameContext->getDrawList()->m_lightData.numLight = 0;
		{
			ZE::Handle handleGatherLight("EventGatherLight", sizeof(ZE::Event_GATHER_LIGHT));
			ZE::Event_GATHER_LIGHT* eventGatherLight = new(handleGatherLight) Event_GATHER_LIGHT();
			_gameContext->getEventDispatcher()->handleEvent(eventGatherLight);
			handleGatherLight.release();
		}

#if ZE_RENDER_MULTITHREAD
		UniqueLock lck(g_drawMutex);
		g_drawReady = true;
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
			double deltaTime = _gameContext->m_renderThreadTimer.ResetAndGetDeltaMS();

			ZELOG(LOG_RENDERING, Log, "Render Delta Time : %.2f ms", deltaTime);

			DrawJob(_gameContext);

			g_drawReady = false;
			while (!g_drawReady) g_drawThreadVariable.wait(lck);
		}
	}

	void DrawJob(GameContext* _gameContext)
	{
		_gameContext->getRenderer()->AcquireRenderThreadOwnership();

		_gameContext->getRenderer()->BeginRender();

		_gameContext->getRenderer()->ClearScreen();

		{
			Matrix4x4 viewMat;
			Matrix4x4 projectionMat;

			ZE::CameraComponent* currentCamera = _gameContext->getCameraManager()->getCurrentCamera();
			if (currentCamera)
			{
				currentCamera->getViewMatrix(viewMat);

				_gameContext->getDrawList()->m_shaderData.setViewMat(viewMat);
				_gameContext->getDrawList()->m_cameraPosition = currentCamera->m_worldTransform.getPos();
				_gameContext->getDrawList()->m_cameraDirection = currentCamera->m_worldTransform.getN();

				ZE::IRenderer* renderer = _gameContext->getRenderer();
				if (currentCamera->m_bUsingOrthoProjection)
				{
					MathOps::CreateOrthoProj(projectionMat, currentCamera->m_orthoWidth / 2.0f, (renderer->GetHeight() / renderer->GetWidth()) * currentCamera->m_orthoWidth * 0.5f, currentCamera->m_near, currentCamera->m_far);
				}
				else
				{
					ZE::MathOps::CreatePerspectiveProjEx(projectionMat, renderer->GetWidth() / renderer->GetHeight(), 45.0f, currentCamera->m_near, currentCamera->m_far);
				}
				_gameContext->getDrawList()->m_shaderData.setProjectionMat(projectionMat);

				_gameContext->getDrawList()->m_lightData.setViewPos(currentCamera->m_worldTransform.getPos());
			}
		}

		_gameContext->getRenderer()->ProcessDrawList(_gameContext->getDrawList());

		_gameContext->getRenderer()->EndRender();

		_gameContext->getDrawList()->Reset();

		_gameContext->getRenderer()->ReleaseRenderThreadOwnership();
	}

}
#include "MainFunctions.h"

#include "Common/GlobalRegistry.h"

#include "Scene/CameraComponent.h"
#include "Scene/RenderComponent.h"
#include "Scene/Light/LightComponent.h"
#include "Renderer/IGPUTexture.h"
#include "Memory/Handle.h"
#include "Events/Events.h"
#include "FileSystem/DirectoryHelper.h"

#include "ResourceManagers/MeshManager.h"
#include "ResourceManagers/MaterialManager.h"

#if Z_RENDER_OPENGL
#include "Renderer/GL/GLRenderZooid.h"
#endif

#include "Platform/Thread.h"

namespace ZE {


	ConditionVariable g_drawThreadVariable;
	Mutex g_drawMutex;
	bool g_drawReady = false;

	void MainSetup(GameContext* _gameContext)
	{

		ZASSERT(ZE::MAIN_THREAD_ID == ZE::getThreadId(), "Not in Main Thread");

		// Construct Memory
		MemoryManager::Construct();

		// Register all classes
		GlobalRegistry::Register();

		// Creating rendering Zooid

		{
#if Z_RENDER_OPENGL
			Handle renderZooidHandle("Render Zooid", sizeof(GLRenderZooid));
			_gameContext->m_renderZooid = new(renderZooidHandle) GLRenderZooid(_gameContext);
#endif
			_gameContext->m_renderZooid->Init();
			_gameContext->m_renderer = _gameContext->m_renderZooid->GetRenderer();
		}

		ShaderManager::Init(_gameContext);
		_gameContext->m_shaderManager = ShaderManager::getInstance();

		BufferManager::Init(_gameContext);
		_gameContext->m_bufferManager = BufferManager::getInstance();
		
		TextureManager::Init(_gameContext);
		_gameContext->m_textureManager = TextureManager::getInstance();

		MaterialManager::Init();
		_gameContext->m_materialManager = MaterialManager::getInstance();

		MeshManager::Init();
		_gameContext->m_meshManager = MeshManager::getInstance();

		{
			Handle handle("DrawList", sizeof(DrawList));
			_gameContext->m_drawList = new(handle) DrawList;
			_gameContext->m_drawList->Setup();
		}

		// Create Main Event Dispatcher
		{
			Handle handle("EventDispatcher", sizeof(EventDispatcher));
			_gameContext->m_mainEventDispatcher = new(handle) EventDispatcher(_gameContext);
		}

		// Create Root Component and add is as child of Event Dispatcher
		{
			Handle handle("RootComponent", sizeof(SceneComponent));
			_gameContext->m_rootComponent = new (handle) SceneComponent(_gameContext);
			_gameContext->getEventDispatcher()->addChild(_gameContext->m_rootComponent);
			_gameContext->m_rootComponent->setupComponent();
		}

		// Create Input Manager
		{
			Handle handle("InputManager", sizeof(InputManager));
			_gameContext->m_inputManager = new (handle) InputManager(_gameContext);
			_gameContext->m_inputManager->setupComponent();
		}

		CameraManager::Init(_gameContext);
		_gameContext->m_cameraManager = CameraManager::GetInstance();
		
		// Put Sample Directional Light
		{
			Handle hDirLight("Directional Light", sizeof(LightComponent));
			LightComponent* pDirLight = new(hDirLight) LightComponent(_gameContext, DIRECTIONAL_LIGHT);
			pDirLight->setupComponent();

			_gameContext->getRootComponent()->addChild(pDirLight);
		}

		// Put sample boxes
		{
			Handle hRenderComp("Sample Crate", sizeof(RenderComponent));
			RenderComponent* pRenderComp = new(hRenderComp) RenderComponent(_gameContext);
			

			pRenderComp->setupComponent();
			pRenderComp->fromFile(GetPackageAssetPath("Basic", "Mesh", "Crate.meshz").c_str());
			pRenderComp->m_worldTransform.setPos(Vector3(1.5f, 0.0f, 0.0f));
			pRenderComp->m_worldTransform.rotateAroundN(DegToRad(45));

			_gameContext->getRootComponent()->addChild(pRenderComp);
		}

		{
			Handle hRenderComp("Sample Crate", sizeof(RenderComponent));
			RenderComponent* pRenderComp = new(hRenderComp) RenderComponent(_gameContext);
			pRenderComp->m_bHighlight = true;

			pRenderComp->setupComponent();
			pRenderComp->fromFile(GetPackageAssetPath("Basic", "Mesh", "Crate.meshz").c_str());
			pRenderComp->m_worldTransform.setPos(Vector3(-1.5f, 0.0f, 0.0f));
			pRenderComp->m_worldTransform.rotateAroundU(DegToRad(45));

			_gameContext->getRootComponent()->addChild(pRenderComp);
		}

		{
			Handle hRenderComp("Sample Crate", sizeof(RenderComponent));
			RenderComponent* pRenderComp = new(hRenderComp) RenderComponent(_gameContext);

			pRenderComp->setupComponent();
			pRenderComp->fromFile(GetPackageAssetPath("Basic", "Mesh", "Crate.meshz").c_str());
			pRenderComp->m_worldTransform.setPos(Vector3(0.0f, 0.0f, 0.0f));
			pRenderComp->m_worldTransform.rotateAroundV(DegToRad(45));

			_gameContext->getRootComponent()->addChild(pRenderComp);
		}

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
		BufferManager::Destroy();
		ShaderManager::Destroy();
		TextureManager::Destroy();

		_gameContext->m_renderZooid->Destroy();

		MemoryManager::Deconstruct();
	}

	void MainThreadJob(GameContext* _gameContext)
	{
		double deltaTime = _gameContext->m_mainTimer.ResetAndGetDeltaMS();

		ZEINFO("Delta Time : %.2f ms", deltaTime);

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

		while (!g_drawReady)
		{
			g_drawThreadVariable.wait(lck);
		}

		while (!_gameContext->getRenderer()->IsClose())
		{
			ZEINFO("Start Draw");
			DrawJob(_gameContext);
			ZEINFO("End Draw");
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

				ZE::IRenderer* renderer = _gameContext->getRenderer();
				//ZE::MathOps::CreatePerspectiveProj(projectionMat, renderer->GetWidth(), renderer->GetHeight(), currentCamera->m_near, currentCamera->m_far);
				ZE::MathOps::CreatePerspectiveProjEx(projectionMat, renderer->GetWidth() / renderer->GetHeight(), 45.0f, currentCamera->m_near, currentCamera->m_far);
				//ZE::MathOps::CreateOrthoProj(projectionMat, 1.0f * renderer->GetWidth() / renderer->GetHeight(), 1.0f, currentCamera->m_near, currentCamera->m_far);

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
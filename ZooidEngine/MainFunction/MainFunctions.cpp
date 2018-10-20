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
#include "ResourceManagers/AnimationManager.h"
#include "ResourceManagers/FontManager.h"

#include "Renderer/DebugRenderer.h"
#include "Renderer/IGPUFrameBuffer.h"
#include "Renderer/ShaderData.h"

#include "scene/Light/LightComponent.h"

// TODO for NVIDIA Optimus :  This enable the program to use NVIDIA instead of integrated Intel graphics
#if WIN32 || WIN64
extern "C"
{
#include <windows.h>
	_declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
}
#endif

namespace ZE 
{
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
			_gameContext->m_rootComponent->setObjectName("Root Component");
			_gameContext->m_rootComponent->setupComponent();
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
			_gameContext->m_mainEventDispatcher->addChild(_gameContext->m_debugRenderer);
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
		double deltaTime = _gameContext->m_mainTimer.ResetAndGetDeltaMS();
		float deltaSeconds = static_cast<Float32>(deltaTime * 0.001f);

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

#if ZE_RENDER_MULTITHREAD
		while (g_drawReady) {}
#endif

		// Draw Debug Text
		DebugRenderer::DrawTextScreen("Zooid Engine", Vector2(10.0f, 10.0f), Vector3(1.0f, 0.0f, 0.0f), 0.5f);
		StringFunc::PrintToString(StringFunc::Buffer, STRING_FUNC_BUFFER_SIZE, "Delta Time: %.2f ms; FPS: %d", deltaTime, static_cast<int>(1000.0f / deltaTime));
		DebugRenderer::DrawTextScreen(StringFunc::Buffer, Vector2(10.0f, _gameContext->getRenderer()->GetHeight() - 30.0f), Vector3(1.0f, 1.0f, 0.0f), 0.5f);

		// Draw World Text
		//Matrix4x4 mat;
		//mat.translate(Vector3(0.0f, 0.1f, 0.0f));
		//DebugRenderer::DrawTextWorld("Zooid Engine", mat);

		// Draw Base Lines
		DebugRenderer::DrawMatrixBasis(Matrix4x4());

		// Handle Event_GATHER_RENDER
		{
			Handle handleGatherRender("EventGatherRender", sizeof(Event_GATHER_RENDER));
			Event_GATHER_RENDER* pEvent = new(handleGatherRender) Event_GATHER_RENDER();
			_gameContext->getEventDispatcher()->handleEvent(pEvent);
			handleGatherRender.release();
		}

		// Handle Event_GATHER_LIGHT
		_gameContext->getDrawList()->m_lightData.NumLight = 0;
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

			//ZELOG(LOG_RENDERING, Log, "Render Delta Time : %.2f ms", deltaTime);

			DrawJob(_gameContext);

			g_drawReady = false;
			while (!g_drawReady) g_drawThreadVariable.wait(lck);
		}
	}

	void DrawJob(GameContext* _gameContext)
	{
		IRenderer* renderer = _gameContext->getRenderer();
		DrawList* drawList = _gameContext->getDrawList();

		renderer->AcquireRenderThreadOwnership();

		// For each Light render Shadows
		for (UInt32 iShadowData = 0; iShadowData < drawList->m_lightShadowSize; iShadowData++)
		{
			LightShadowMapData& shadowMapData = drawList->m_lightShadowMapData[iShadowData];
			LightStruct& light = drawList->m_lightData.lights[shadowMapData.lightIndex];
			
			if(!shadowMapData.dynamicShadowFrameBuffer) { continue; }

			// TODO Set appropriate shader data
			Matrix4x4 view;
			Matrix4x4 proj;

			if (light.Type == LightType::DIRECTIONAL_LIGHT)
			{
				MathOps::LookAt(view, light.getDirection() * -5.0f, Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, 1.0f, 0.0f));
				MathOps::CreateOrthoProjEx(proj, -5.0f, 5.0f, -5.0f, 5.0f, 0.1f, 10.0f);
			}
			else if (light.Type == LightType::SPOT_LIGHT)
			{
				MathOps::LookAt(view, light.getPosition(), light.getPosition() + light.getDirection(), Vector3(0.0f, 1.0f, 0.0f));
				MathOps::CreatePerspectiveProjEx(proj, 1.0f,  2.0 * RadToDeg(acos(light.OuterCutOff)), 0.1f, 10.0f);
			}
			
			light.setViewProjMatrix(view * proj);

			drawList->m_shaderData.setViewMat(view);
			drawList->m_shaderData.setProjectionMat(proj);
			drawList->m_mainConstantBuffer->bindAndRefresh();

			UInt32 shadowMapIndex = 0;

			// Set Shader Chain for this light pass
			for (UInt32 iDynObj = 0; iDynObj < drawList->m_staticShadowObjSize; iDynObj++)
			{
				ShaderAction& shaderAction = drawList->m_staticShadowObjList[iDynObj];
				if (shaderAction.isSkin())
				{
					shaderAction.setShaderChain(shadowMapData.skinnedShaderChain);
				}
				else
				{
					shaderAction.setShaderChain(shadowMapData.normalShaderChain);
				}

				// TODO Set the var if any
			}

			if (!shadowMapData.staticShadowTexture)
			{
				for (UInt32 iDynObj = 0; iDynObj < drawList->m_dynamicShadowObjSize; iDynObj++)
				{
					ShaderAction& shaderAction = drawList->m_dynamicShadowObjList[iDynObj];
					if (shaderAction.isSkin())
					{
						shaderAction.setShaderChain(shadowMapData.skinnedShaderChain);
					}
					else
					{
						shaderAction.setShaderChain(shadowMapData.normalShaderChain);
					}

					// TODO Set the var if any
				}
			}
			else
			{
				drawList->m_shadowMap[drawList->m_shadowMapSize] = shadowMapData.staticShadowTexture;
				light.shadowMapIndices[shadowMapIndex++] = drawList->m_shadowMapSize;
				drawList->m_shadowMapSize++;
			}
			
			drawList->m_shadowMap[drawList->m_shadowMapSize] = shadowMapData.dynamicShadowTexture;
			light.shadowMapIndices[shadowMapIndex++] = drawList->m_shadowMapSize;
			drawList->m_shadowMapSize++;

			while (shadowMapIndex < 4)
			{
				light.shadowMapIndices[shadowMapIndex++] = -1;
			}

			// Process Shadow Render
			shadowMapData.dynamicShadowFrameBuffer->bind(); // Bind Frame Buffer

			renderer->ProcessShadowMapList(drawList, !shadowMapData.staticShadowTexture);

			shadowMapData.dynamicShadowFrameBuffer->unbind(); // Unbind frame buffer

		}

		renderer->BeginRender();

		renderer->ClearScreen();

		{
			Matrix4x4 viewMat;
			Matrix4x4 projectionMat;

			ZE::CameraComponent* currentCamera = _gameContext->getCameraManager()->getCurrentCamera();
			if (currentCamera)
			{
				currentCamera->getViewMatrix(viewMat);

				_gameContext->getDrawList()->m_shaderData.setViewMat(viewMat);
				_gameContext->getDrawList()->m_cameraPosition = currentCamera->getWorldPosition();
				_gameContext->getDrawList()->m_cameraDirection = currentCamera->getForwardVector();

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

				_gameContext->getDrawList()->m_lightData.setViewPos(currentCamera->getWorldPosition());
			}
		}

		_gameContext->getRenderer()->ProcessDrawList(_gameContext->getDrawList());

		_gameContext->getRenderer()->EndRender();

		_gameContext->getDrawList()->Reset();

		_gameContext->getRenderer()->ReleaseRenderThreadOwnership();
	}

}
#include "MainFunctions.h"

#include "../Common/GlobalRegistry.h"

#include "Scene/CameraComponent.h"
#include "Scene/Light/LightComponent.h"
#include "Renderer/GPUTexture.h"
#include "Memory/Handle.h"
#include "Events/Events.h"
#include "FileSystem/DirectoryHelper.h"

#include "ResourceManagers/MeshManager.h"
#include "ResourceManagers/MaterialManager.h"

namespace ZE {

	void MainSetup(GameContext* _gameContext)
	{
		// Construct Memory
		MemoryManager::Construct();

		// Register all classes
		GlobalRegistry::Register();

		{
			Handle handle("RENDERER", sizeof(ZE::GLRenderer));
			_gameContext->m_renderer = new(handle) ZE::GLRenderer();
			_gameContext->m_renderer->Setup();
		}

		ShaderManager::Init();
		_gameContext->m_shaderManager = ShaderManager::getInstance();

		BufferManager::Init(_gameContext);
		_gameContext->m_bufferManager = BufferManager::getInstance();
		
		TextureManager::Init();
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

	}

	void MainClean(GameContext* _gameContext)
	{
		CameraManager::Destroy();
		BufferManager::Destroy();
		ShaderManager::Destroy();
		TextureManager::Destroy();

		_gameContext->m_renderer->Clean();

		MemoryManager::Deconstruct();
	}

	void MainThreadJob(GameContext* _gameContext)
	{
		Matrix4x4 modelMat;

		{
			modelMat.translate(Vector3(-1.f, 0.0f, 0.0f));

			ZE::ShaderAction& shaderAction = _gameContext->getDrawList()->getNextShaderAction();
			ZE::ShaderChain* shader = ZE::ShaderManager::getInstance()->getShaderChain(Z_SHADER_CHAIN_3D_DEFAULT_LIT);

			shaderAction.SetShaderAndBuffer(shader, ZE::BufferManager::getInstance()->getResource<GPUBufferArray>(GetPackageAssetPath("Basic", "VertexBuffer", "Cube.vbuff").c_str()));
			shaderAction.m_vertexSize = 36;
			shaderAction.SetShaderMatVar("modelMat", modelMat);
			ZE::GPUTexture* pGPUTexture = _gameContext->getTextureManager()->getResource<ZE::GPUTexture>(GetPackageAssetPath("Basic", "Texture", "container2.png").c_str());
			shaderAction.SetShaderTextureVar("material.diffuseMap", pGPUTexture, 0);
			shaderAction.SetShaderFloatVar("material.shininess", 32.0f);
			shaderAction.SetConstantsBlockBuffer("shader_data", _gameContext->getDrawList()->m_mainConstantBuffer);
			shaderAction.SetConstantsBlockBuffer("light_data", _gameContext->getDrawList()->m_lightConstantBuffer);
		}

		{
			ZE::ShaderAction& shaderAction = _gameContext->getDrawList()->getNextShaderAction();
			ZE::ShaderChain* shader = ZE::ShaderManager::getInstance()->getShaderChain(2);

			shaderAction.SetShaderAndBuffer(shader, ZE::BufferManager::getInstance()->m_GPUBufferArrays[2]);
			shaderAction.m_vertexSize = 36;
			shaderAction.SetShaderMatVar("modelMat", Matrix4x4());
			shaderAction.SetConstantsBlockBuffer("shader_data", _gameContext->getDrawList()->m_mainConstantBuffer);
			shaderAction.SetConstantsBlockBuffer("light_data", _gameContext->getDrawList()->m_lightConstantBuffer);
		}

		// Handle Event_Update
		{
			ZE::Handle handleUpdate("EventUpdate", sizeof(ZE::Event_UPDATE));
			ZE::Event_UPDATE* eventUpdate = new(handleUpdate) ZE::Event_UPDATE();
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

		// Handle Event_GATHER_LIGHT
		_gameContext->getDrawList()->m_lightData.numLight = 0;
		{
			ZE::Handle handleGatherLight("EventGatherLight", sizeof(ZE::Event_GATHER_LIGHT));
			ZE::Event_GATHER_LIGHT* eventGatherLight = new(handleGatherLight) Event_GATHER_LIGHT();
			_gameContext->getEventDispatcher()->handleEvent(eventGatherLight);
			handleGatherLight.release();
		}

		_gameContext->getRenderer()->ProcessDrawList(_gameContext->getDrawList());

		_gameContext->getRenderer()->EndRender();

		_gameContext->getDrawList()->Reset();
	}

}
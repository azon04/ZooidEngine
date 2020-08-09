#include "SceneEditor.h"

#include "ZEGameContext.h"

#include "Platform/Platform.h"

#include "ZooidEngine/Scene/SceneManager.h"
#include "ZooidEngine/FileSystem/DirectoryHelper.h"
#include "ZooidEngine/ResourceManagers/AnimationManager.h"

#include "ZooidEngine/Animation/AnimationComponent.h"
#include "ZooidEngine/Animation/Animation.h"

#include "ZooidEngine/Scene/CameraManager.h"
#include "ZooidEngine/Scene/CameraComponent.h"

#include "ZooidEngine/UI/ZooidUI.h"
#include "ZooidEngine/UI/DataProviderHelpers.h"

#include "ZooidEngine/Renderer/DebugRenderer.h"
#include "ZooidEngine/Renderer/IRenderer.h"
#include "ZooidEngine/Scene/RenderComponent.h"

#include "ZooidEngine/Utils/DebugOptions.h"

namespace ZE
{
	void SceneEditor::Setup(GameContext* _gameContext)
	{
		int argCount = Platform::GetArgCount();
		String scenePath(1024);

		if (argCount >= 1)
		{
			Platform::GetArgByIndex(0, scenePath.c_str());
		}

		_gameContext->getSceneManager()->loadSceneFile(GetResourcePath(scenePath.const_str()).c_str());

		{
			CameraComponent* cameraComp = CameraManager::GetInstance()->getCurrentCamera();
			Quaternion q;
			Matrix4x4 transform;
			transform.fromQuaternion(q);
			transform.setPos(Vector3(0.0f, 1.0f, 5.0f));
			cameraComp->setWorldTransform(transform);
		}

		gDebugOptions.bShowFPSStats = true;
	}

	void SceneEditor::Tick(GameContext* _gameContext, float deltaTime)
	{
		SceneManager* sceneManger = _gameContext->getSceneManager();
		static ArrayListProvider<String> listProvider;
		listProvider.setArray(sceneManger->getComponentList());

		if (ZE::UI::BeginPanel("Scene", ZE::UIRect(ZE::UIVector2(gRenderWidth - 310.0f, 10.0f), ZE::UIVector2(300.0f, 400.0f))))
		{
			if (UI::DoSelectionListView("SceneList", ZE::UIRect(ZE::UIVector2(0.0f, 0.0f), ZE::UIVector2(260.0f, 370.0f)), &listProvider, m_selectedIndex))
			{
				if (m_selectedIndex > -1)
				{
					if (m_selectedRenderComponent && m_selectedRenderComponent->IsA(RenderComponent::GetClassID()))
					{
						RenderComponent* renderComp = (RenderComponent*)m_selectedRenderComponent;
						renderComp->setHighLight(false);
					}

					Handle compHandle = sceneManger->getCompByName(listProvider.getItemAt(m_selectedIndex));
					Component* pComp = compHandle.isValid() ? compHandle.getObject<Component>() : nullptr;
					m_selectedRenderComponent = (pComp && pComp->IsA(SceneComponent::GetClassID())) ? (SceneComponent*)pComp : nullptr;
				}
				else
				{
					m_selectedRenderComponent = nullptr;
				}
			}
			ZE::UI::EndPanel();
		}

		if (m_selectedRenderComponent)
		{
			if (m_selectedRenderComponent->IsA(RenderComponent::GetClassID()))
			{
				RenderComponent* renderComp = (RenderComponent*)m_selectedRenderComponent;
				renderComp->setHighLight(true);
			}
			else
			{
				static Matrix4x4 mat;
				mat.setPos(m_selectedRenderComponent->getWorldPosition());
				DebugRenderer::DrawMatrixBasis(mat);
			}
		}
	}

	void SceneEditor::Clean(GameContext* _gameContext)
	{

	}

	Application* Application::GetApplication()
	{
		static SceneEditor application;
		return &application;
	}

}

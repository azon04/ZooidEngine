#include "SceneManager.h"
#include "ZEGameContext.h"

#include "FileSystem/FileReader.h"
#include "FileSystem/DirectoryHelper.h"

#include "Scene/Light/LightComponent.h"
#include "Scene/RenderComponent.h"
#include "Scene/CameraComponent.h"
#include "Collision/BoxComponent.h"
#include "Collision/SphereComponent.h"
#include "Collision/CapsuleComponent.h"
#include "Animation/AnimationComponent.h"
#include "Animation/AnimationSM.h"

#include "ResourceManagers/AnimationManager.h"

#include "Utils/StringFunc.h"

#include "Memory/Handle.h"

#include "SceneComponent.h"

namespace ZE
{
	IMPLEMENT_CLASS_1(SceneManager, Component);

	SceneManager* SceneManager::s_instance = nullptr;

	void SceneManager::LoadSceneFile(const char* filePath)
	{
		LoadSceneFileToComp(filePath, m_gameContext->getRootComponent());
	}

	Handle SceneManager::CreateSceneComponentByName(const char* componentTypeName)
	{
		if (StringFunc::Compare(componentTypeName, LightComponent::GetClassName()) == 0)
		{
			Handle h("Light Component", sizeof(LightComponent));
			new(h) LightComponent(m_gameContext);
			return h;
		}
		else if (StringFunc::Compare(componentTypeName, RenderComponent::GetClassName()) == 0)
		{
			Handle h("Render Component", sizeof(RenderComponent));
			new (h) RenderComponent(m_gameContext);
			return h;
		}
		else if (StringFunc::Compare(componentTypeName, CameraComponent::GetClassName()) == 0)
		{
			Handle h("Component", sizeof(CameraComponent));
			new(h) CameraComponent(m_gameContext);
			return h;
		}
		else if (StringFunc::Compare(componentTypeName, BoxComponent::GetClassName()) == 0)
		{
			Handle h("Component", sizeof(BoxComponent));
			new(h) BoxComponent(m_gameContext);
			return h;
		}
		else if (StringFunc::Compare(componentTypeName, SphereComponent::GetClassName()) == 0)
		{
			Handle h("Component", sizeof(SphereComponent));
			new(h) SphereComponent(m_gameContext);
			return h;
		}
		else if (StringFunc::Compare(componentTypeName, CapsuleComponent::GetClassName()) == 0)
		{
			Handle h("Component", sizeof(CapsuleComponent));
			new(h) CapsuleComponent(m_gameContext);
			return h;
		}
		else
		{
			Handle h("Component", sizeof(SceneComponent));
			new(h) SceneComponent(m_gameContext);
			return h;
		}
	}

	void SceneManager::LoadSceneFileToComp(const char* filePath, Component* parent)
	{
		FileReader fileReader(filePath);
		if (!fileReader.isValid())
		{
			ZEINFO("Failed to load scene file: %s", filePath);
		}

		char buff[256];
		
		// #TODO read Scene Name
		fileReader.readNextString(buff);
		fileReader.readNextString(buff);

		// #TODO read file version
		fileReader.readNextString(buff);
		fileReader.readNextString(buff);

		// Read component count
		fileReader.readNextString(buff);
		Int32 compCount = fileReader.readNextInt();
		for (Int32 i = 0; i < compCount; i++)
		{
			LoadSceneComponentToComp(&fileReader, parent);
		}
	}

	void SceneManager::LoadSceneComponentToComp(FileReader* fileReader, Component* parent)
	{
		char buff[256];
		// Component type
		fileReader->readNextString(buff);
		Handle compHandle = CreateSceneComponentByName(buff);
		SceneComponent* pComp = compHandle.getObject<SceneComponent>();

		// #TODO component name
		fileReader->readNextString(buff);
		pComp->setObjectName(String(buff));

		m_componentMap.put(buff, compHandle);

		// BEGIN
		fileReader->readNextString(buff);

		fileReader->readNextString(buff);
		while (StringFunc::Compare(buff, "END") != 0)
		{
			if (StringFunc::Compare(buff, "R") == 0)
			{
				// Read Rotation
				Float32 x, y, z;
				x = fileReader->readNextFloat();
				y = fileReader->readNextFloat();
				z = fileReader->readNextFloat();

				pComp->m_worldTransform.rotateAroundU(DegToRad(x));
				pComp->m_worldTransform.rotateAroundV(DegToRad(y));
				pComp->m_worldTransform.rotateAroundN(DegToRad(z));
			}
			else if (StringFunc::Compare(buff, "S") == 0)
			{
				// Read Scale
				Vector3 scale;
				scale.m_x = fileReader->readNextFloat();
				scale.m_y = fileReader->readNextFloat();
				scale.m_z = fileReader->readNextFloat();

				pComp->m_worldTransform.scale(scale);
			}
			else if (StringFunc::Compare(buff, "T") == 0)
			{
				// Read Pos
				Vector3 pos;
				pos.m_x = fileReader->readNextFloat();
				pos.m_y = fileReader->readNextFloat();
				pos.m_z = fileReader->readNextFloat();

				pComp->m_worldTransform.setPos(pos);
			}
			else if (StringFunc::Compare(buff, "Mesh") == 0)
			{
				// Read Mesh
				fileReader->readNextString(buff);
				if (RenderComponent* pRendComp = (RenderComponent*) pComp)
				{
					pRendComp->fromFile(GetResourcePath(buff).c_str());
				}
			}
			else if (StringFunc::Compare(buff, "LightType") == 0)
			{
				// Read Light Type
				int lightType = fileReader->readNextInt();
				if (LightComponent* pLightComp = (LightComponent*)pComp)
				{
					pLightComp->m_lightType = (LightType)lightType;
				}
			}
			else if (StringFunc::Compare(buff, "Children") == 0)
			{
				// Read Children of this comp
				Int32 count = fileReader->readNextInt();
				for (Int32 i = 0; i < count; i++)
				{
					LoadSceneComponentToComp(fileReader, pComp);
				}
			}
			else if (StringFunc::Compare(buff, "Scene") == 0)
			{
				// Read Scene
				fileReader->readNextString(buff);
				LoadSceneFileToComp(GetResourcePath(buff).c_str(), pComp);
			}
			else if (StringFunc::Compare(buff, "Physics") == 0)
			{
				fileReader->readNextString(buff);
				if (RenderComponent* pRendComp = dynamic_cast<RenderComponent*>(pComp))
				{
					pRendComp->m_bStatic = StringFunc::Compare(buff, "true") != 0;
				}
			}
			else if (StringFunc::Compare(buff, "TriggerOnly") == 0)
			{
				fileReader->readNextString(buff);
				if (CollisionComponent* pCollisionComp = dynamic_cast<CollisionComponent*>(pComp))
				{
					pCollisionComp->setTrigger(StringFunc::Compare(buff, "true") == 0);
				}
			}
			else if (StringFunc::Compare(buff, "Extent") == 0)
			{
				if (BoxComponent* pBoxComponent = dynamic_cast<BoxComponent*>(pComp))
				{
					pBoxComponent->m_halfExtent.setX(fileReader->readNextFloat());
					pBoxComponent->m_halfExtent.setY(fileReader->readNextFloat());
					pBoxComponent->m_halfExtent.setZ(fileReader->readNextFloat());
				}
			}
			else if (StringFunc::Compare(buff, "Radius") == 0)
			{
				if (SphereComponent* pSphereComp = dynamic_cast<SphereComponent*>(pComp))
				{
					pSphereComp->m_radius = fileReader->readNextFloat();
				}
				else if (CapsuleComponent* pCapsuleComp = dynamic_cast<CapsuleComponent*>(pComp))
				{
					pCapsuleComp->m_radius = fileReader->readNextFloat();
				}
			}
			else if (StringFunc::Compare(buff, "Height") == 0)
			{
				if (CapsuleComponent* pCapsuleComp = dynamic_cast<CapsuleComponent*>(pComp))
				{
					pCapsuleComp->m_height = fileReader->readNextFloat();
				}
			}
			else if (StringFunc::Compare(buff, "Animation") == 0)
			{
				Handle hAnimationComp("Animation Component", sizeof(AnimationComponent));
				AnimationComponent* pAnimationComp = new(hAnimationComp) AnimationComponent(m_gameContext);
				pAnimationComp->setupComponent();
				
				// Read Animation path
				fileReader->readNextString(buff);
				Handle hAnimClip = AnimationManager::GetInstance()->loadResource(GetResourcePath(buff).c_str());
				bool bLoop = fileReader->readNextInt() == 1;
				float rate = fileReader->readNextFloat();

				if (hAnimClip.isValid())
				{
					AnimationClip* pAnimClip = hAnimClip.getObject<AnimationClip>();
					pAnimationComp->playAnimationClip(pAnimClip, bLoop, rate);
				}

				pComp->addChild(pAnimationComp);
			}
			else if (StringFunc::Compare(buff, "AnimationState") == 0)
			{
				Handle hAnimationSM("Animation SM", sizeof(AnimationSM));
				AnimationSM* pAnimSM = new(hAnimationSM) AnimationSM(m_gameContext);
				pAnimSM->setupComponent();

				// Read Animation State Def
				fileReader->readNextString(buff);
				pAnimSM->readAnimStateDef(GetResourcePath(buff).c_str());

				pComp->addChild(pAnimSM);
			}

			fileReader->readNextString(buff);
		}

		pComp->setupComponent();
		parent->addChild(pComp);
	}

	ZE::Handle SceneManager::getCompByName(const char* name)
	{
		if (m_componentMap.hasKey(name))
		{
			return m_componentMap[name];
		}

		return Handle();
	}

	void SceneManager::Init(GameContext* _gameContext)
	{
		Handle handle("SceneManager", sizeof(SceneManager));
		s_instance = new(handle) SceneManager(_gameContext);
	}

	void SceneManager::Destroy()
	{

	}

}

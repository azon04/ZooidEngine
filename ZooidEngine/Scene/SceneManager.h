#ifndef __Z_SCENE_MANAGER_H__
#define __Z_SCENE_MANAGER_H__

#include "GameObjectModel/Component.h"
#include "Utils/StringName.h"

namespace ZE
{
	class FileReader;

	class SceneManager : public Component 
	{
		DEFINE_CLASS(SceneManager)
	
	public:

		SceneManager(GameContext* gameContext) : Component(gameContext) {}
		virtual ~SceneManager() {}

		void loadSceneFile(const char* filePath);
		void loadSceneFileToComp(const char* filePath, Component* parent);
		void loadSceneComponentToComp(FileReader* reader, Component* parent);
		void addComponent(const String& name, Handle componentHandle);

		Array<String>& getComponentList() { return m_cachedCompNames; }
		Handle getCompByName(const char* name);

		static void Init(GameContext* _gameContext);
		static void Destroy();

		static SceneManager* GetInstance() { return s_instance; }

	private:
		Handle CreateSceneComponentByName(const char* componentTypeName);
		HashMap<String, Handle> m_componentMap;
		Array<String> m_cachedCompNames;
		static SceneManager* s_instance;

	public:
		String m_currentSceneFile;
		
		// Tokens
		static const StringName BEGIN;
		static const StringName END;
		static const StringName ROTATION;
		static const StringName SCALE;
		static const StringName TRANSLATION;
		static const StringName MESH;
		static const StringName MATERIAL;
		static const StringName LIGHT_TYPE;
		static const StringName ATT_DISTANCE;
		static const StringName ATT_CONSTANT;
		static const StringName ATT_LINEAR;
		static const StringName ATT_QUADRATIC;
		static const StringName CHILDREN;
		static const StringName SCENE;
		static const StringName PHYSICS;
		static const StringName TRIGGER_ONLY;
		static const StringName EXTENT;
		static const StringName RADIUS;
		static const StringName OUTER_RADIUS;
		static const StringName HEIGHT;
		static const StringName ANIMATION;
		static const StringName ANIMATION_STATE;
		static const StringName	TEXTURE;
	};
}
#endif

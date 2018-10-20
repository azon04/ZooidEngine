#ifndef __Z_SCENE_MANAGER_H__
#define __Z_SCENE_MANAGER_H__

#include "GameObjectModel/Component.h"

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

		Handle getCompByName(const char* name);

		static void Init(GameContext* _gameContext);
		static void Destroy();

		static SceneManager* GetInstance() { return s_instance; }

	private:
		Handle CreateSceneComponentByName(const char* componentTypeName);
		HashMap<String, Handle> m_componentMap;
		static SceneManager* s_instance;

	public:
		String m_currentSceneFile;
		
	};
}
#endif
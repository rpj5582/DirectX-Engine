#pragma once

#include "Scene.h"

class SceneManager
{
public:
	SceneManager();
	~SceneManager();

	unsigned int getSceneCount() const;

	static Scene* getActiveScene();

	static bool addScene(Scene* scene);

	static bool loadScene(Scene* scene);
	static bool loadScene(std::string name);
	static bool loadScene(unsigned int index);

	static void saveActiveScene();
	static void clearActiveScene();

	static std::string d_sceneNameField;

private:
	static SceneManager* m_instance;

	std::vector<Scene*> m_sceneList;
	Scene* m_activeScene;
};


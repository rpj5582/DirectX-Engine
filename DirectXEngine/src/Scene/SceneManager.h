#pragma once

#include "Scene.h"

class SceneManager
{
public:
	SceneManager(HWND hWnd);
	~SceneManager();

	static Scene* getActiveScene();

	static void newScene();
	static bool loadScene();
	static bool saveActiveScene();
	static bool saveActiveSceneAs();

private:
	void deleteActiveScene();

	static SceneManager* m_instance;
	HWND m_hWnd;

	Scene* m_activeScene;
};


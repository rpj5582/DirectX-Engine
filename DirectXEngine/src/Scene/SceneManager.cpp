#include "SceneManager.h"

#include "../Debug/Debug.h"
#include "../Util.h"

SceneManager* SceneManager::m_instance = nullptr;

SceneManager::SceneManager(HWND hWnd)
{
	if (!m_instance) m_instance = this;
	else return;

	m_hWnd = hWnd;

	m_activeScene = new Scene();
	m_activeScene->init();
}

SceneManager::~SceneManager()
{
	deleteActiveScene();
}

Scene* SceneManager::getActiveScene()
{
	return m_instance->m_activeScene;
}

void SceneManager::newScene()
{
	m_instance->deleteActiveScene();
	m_instance->m_activeScene = new Scene();
	m_instance->m_activeScene->init();
}

bool SceneManager::loadScene()
{
	std::string filepath = Util::loadFileDialog(m_instance->m_hWnd, "JSON Scene File\0*.json");
	if (filepath == "") return false;

	newScene();

	return m_instance->m_activeScene->loadFromJSON(filepath);
}

bool SceneManager::saveActiveScene()
{
	if (m_instance->m_activeScene->hasFilePath())
	{
		m_instance->m_activeScene->saveToJSON();
		return true;
	}
	else
		return saveActiveSceneAs();
}

bool SceneManager::saveActiveSceneAs()
{
	std::string filepath = Util::saveFileDialog(m_instance->m_hWnd, "JSON Scene File\0*.json");
	if (filepath == "") return false;

	m_instance->m_activeScene->saveToJSON(filepath);
	return true;
}

void SceneManager::deleteActiveScene()
{
	if (!m_activeScene) return;

	delete m_activeScene;
	m_activeScene = nullptr;
}

#include "SceneManager.h"

SceneManager* SceneManager::m_instance = nullptr;
std::string SceneManager::d_sceneNameField = "";

SceneManager::SceneManager()
{
	if (!m_instance) m_instance = this;
	else return;

	m_sceneList = std::vector<Scene*>();
	m_activeScene = nullptr;
}

SceneManager::~SceneManager()
{
	for (unsigned int i = 0; i < m_sceneList.size(); i++)
	{
		delete m_sceneList[i];
	}
	m_sceneList.clear();
	m_activeScene = nullptr;
}

unsigned int SceneManager::getSceneCount() const
{
	return m_sceneList.size();
}

Scene* SceneManager::getActiveScene()
{
	return m_instance->m_activeScene;
}

bool SceneManager::addScene(Scene* scene)
{
	if (!scene->init())
	{
		Debug::error("Failed to initialize scene.");
		return false;
	}
	
	m_instance->m_sceneList.push_back(scene);
	return true;
}

bool SceneManager::loadScene(Scene* scene)
{
	for (unsigned int i = 0; i < m_instance->m_sceneList.size(); i++)
	{
		if (m_instance->m_sceneList[i] == scene)
		{
			return loadScene(i);
		}
	}

	Debug::warning("Failed to load scene because it was not found in the scene list.");
	return false;
}

bool SceneManager::loadScene(std::string name)
{
	for (unsigned int i = 0; i < m_instance->m_sceneList.size(); i++)
	{
		if (m_instance->m_sceneList[i]->getName() == name)
		{
			return loadScene(i);
		}
	}

	Debug::warning("Failed to load scene with name " + name + " because it was not found in the scene list.");
	return false;
}

bool SceneManager::loadScene(unsigned int index)
{
	if (index >= m_instance->m_sceneList.size())
	{
		Debug::warning("Failed to load scene at index " + std::to_string(index) + " because it is outside the bounds of the scene list.");
		return false;
	}

	if (!m_instance->m_sceneList[index]->loadFromJSON())
	{
		Debug::warning("Failed to load scene " + m_instance->m_sceneList[index]->getName() + ".");
		return false;
	}

	m_instance->m_activeScene = m_instance->m_sceneList[index];

	Debug::sceneDebugWindow->setupControls();
	Debug::entityDebugWindow->setupControls();
	Debug::assetDebugWindow->setupControls();

	return true;
}

void SceneManager::saveActiveScene()
{
	if (!m_instance->m_activeScene)
	{
		Debug::warning("No active scene to save.");
		return;
	}

	m_instance->m_activeScene->saveToJSON();
}

void SceneManager::clearActiveScene()
{
	if (!m_instance->m_activeScene)
	{
		Debug::warning("No active scene to clear.");
		return;
	}

	m_instance->m_activeScene->clear();
}

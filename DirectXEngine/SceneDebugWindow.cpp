#include "SceneDebugWindow.h"

#include "Scene.h"

SceneDebugWindow::SceneDebugWindow(std::string windowID, std::string windowLabel) : DebugWindow(windowID, windowLabel)
{
	TwDefine((windowID + " position='20 25' size='250 120' ").c_str());
}

SceneDebugWindow::~SceneDebugWindow()
{
}

void SceneDebugWindow::setupControls(Scene* scene)
{
	addButton("NewSceneButton", "New Scene", "''", &newSceneDebugEditor, scene);
	addSeparator("NewSceneSeparator", "''");

	TwAddVarRW(m_window, "SceneInputField", TW_TYPE_STDSTRING, &scene->d_sceneNameField, " label='Scene Name' ");
	addButton("LoadSceneButton", "Load Scene", "''", &loadSceneDebugEditor, scene);
	addButton("SaveSceneButton", "Save Scene", "''", &saveSceneDebugEditor, scene);
}

void TW_CALL newSceneDebugEditor(void* clientData)
{
	Scene* scene = static_cast<Scene*>(clientData);
	TwRemoveAllVars(Debug::entityDebugWindow->getWindow());
	TwRemoveAllVars(Debug::assetDebugWindow->getWindow());
	scene->clear();
	Debug::entityDebugWindow->setupControls(scene);
	Debug::assetDebugWindow->setupControls(scene);
}

void TW_CALL loadSceneDebugEditor(void* clientData)
{
	Scene* scene = static_cast<Scene*>(clientData);
	if (scene->d_sceneNameField.empty())
	{
		Debug::warning("No scene name given. Enter a scene to load.");
		return;
	}

	TwRemoveAllVars(Debug::entityDebugWindow->getWindow());
	TwRemoveAllVars(Debug::assetDebugWindow->getWindow());
	scene->clear();
	Debug::entityDebugWindow->setupControls(scene);
	Debug::assetDebugWindow->setupControls(scene);
	scene->loadFromJSON("Assets/Scenes/" + scene->d_sceneNameField + ".json");
}

void TW_CALL saveSceneDebugEditor(void* clientData)
{
	Scene* scene = static_cast<Scene*>(clientData);
	if (scene->d_sceneNameField.empty())
	{
		Debug::warning("Name the scene before saving.");
		return;
	}
	scene->saveToJSON("Assets/Scenes/" + scene->d_sceneNameField + ".json");
}

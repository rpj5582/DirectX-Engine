#include "SceneDebugWindow.h"

#include "Scene.h"

SceneDebugWindow::SceneDebugWindow(std::string windowID, std::string windowLabel) : DebugWindow(windowID, windowLabel)
{
	TwDefine((windowID + " position='20 50' size='200 150' ").c_str());
}

SceneDebugWindow::~SceneDebugWindow()
{
}

void SceneDebugWindow::setupControls(Scene* scene)
{
	TwAddVarRW(m_window, "SceneInputField", TW_TYPE_STDSTRING, &scene->d_sceneNameField, " label='Scene Name' ");
	addButton("LoadSceneButton", "Load Scene", "''", &loadSceneDebugEditor, scene);
	addButton("SaveSceneButton", "Save Scene", "''", &saveSceneDebugEditor, scene);
	addSeparator("SaveSceneSeparator", "");

	TwAddVarRW(m_window, "AddEntityInputField", TW_TYPE_STDSTRING, &scene->d_entityNameField, " label='Entity Name' ");
	addButton("AddEntityButton", "Add Entity", "''", &addEntityDebugEditor, scene);
	addSeparator("AddEntitySeparator", "");
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
	scene->clear();
	scene->loadFromJSON(scene->d_sceneNameField + ".json");
}

void TW_CALL saveSceneDebugEditor(void* clientData)
{
	Scene* scene = static_cast<Scene*>(clientData);
	if (scene->d_sceneNameField.empty())
	{
		Debug::warning("Name the scene before saving.");
		return;
	}
	scene->saveToJSON(scene->d_sceneNameField + ".json");
}

void TW_CALL addEntityDebugEditor(void* clientData)
{
	Scene* scene = static_cast<Scene*>(clientData);
	if (!scene->d_entityNameField.empty())
		scene->createEntity(scene->d_entityNameField);
	else
		Debug::warning("Could not create entity because no name was given. Please entity a unique name for this entity.");
}

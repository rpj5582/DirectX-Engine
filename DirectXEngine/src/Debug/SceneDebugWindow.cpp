#include "SceneDebugWindow.h"

#include "../Scene/SceneManager.h"

SceneDebugWindow::SceneDebugWindow(std::string windowID, std::string windowLabel) : DebugWindow(windowID, windowLabel)
{
	d_activeSceneNameDisplay = "";

	TwDefine((windowID + " position='20 25' size='250 150' ").c_str());
}

SceneDebugWindow::~SceneDebugWindow()
{
}

void SceneDebugWindow::setupControls()
{
	Scene* activeScene = SceneManager::getActiveScene();
	if (activeScene)
	{
		d_activeSceneNameDisplay = activeScene->getName();
		TwAddVarRO(m_window, "SceneName", TW_TYPE_STDSTRING, &d_activeSceneNameDisplay, " label='Currently Editing:' ");
	}
	else
	{
		d_activeSceneNameDisplay = "No Active Scene!";
		TwAddVarRO(m_window, "SceneName", TW_TYPE_STDSTRING, &d_activeSceneNameDisplay, " label='Currently Editing:' ");
	}

	addSeparator("SceneNameSeparator", "''");

	TwAddVarRW(m_window, "SceneInputField", TW_TYPE_STDSTRING, &SceneManager::d_sceneNameField, " label='Scene Name' ");
	addButton("LoadSceneButton", "Load Scene", "''", &loadSceneDebugEditor, nullptr);
	addSeparator("LoadSceneSeparator", "''");
	addButton("SaveSceneButton", "Save Scene", "''", &saveSceneDebugEditor, nullptr);
	addSeparator("SaveSceneSeparator", "''");
	addButton("ClearSceneButton", "Clear Scene", "''", &newSceneDebugEditor, nullptr);
	addSeparator("ClearSceneSeparator", "''");

	addButton("ToggleDebugIconsButton", "Toggle Debug Icons", "'Settings'", &toggleDebugIconsSceneDebugEditor, nullptr);
	addSeparator("ToggleDebugIconsSeparator", "'Settings'");
	addButton("ShowEntitiesButton", "Expand All Entities", "'Settings'", &showAllEntitiesSceneDebugEditor, nullptr);
	addButton("HideEntitiesButton", "Hide All Entities", "'Settings'", &hideAllEntitiesSceneDebugEditor, nullptr);
}

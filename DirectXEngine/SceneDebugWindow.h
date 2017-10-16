#pragma once
#include "DebugWindow.h"

class Scene;

class SceneDebugWindow : public DebugWindow
{
public:
	SceneDebugWindow(std::string windowID, std::string windowLabel);
	~SceneDebugWindow();

	void setupControls(Scene* scene);
};

void TW_CALL loadSceneDebugEditor(void* clientData);
void TW_CALL saveSceneDebugEditor(void* clientData);
void TW_CALL addEntityDebugEditor(void* clientData);
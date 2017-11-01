#pragma once
#include "DebugWindow.h"

class SceneDebugWindow : public DebugWindow
{
public:
	SceneDebugWindow(std::string windowID, std::string windowLabel);
	~SceneDebugWindow();

	void setupControls() override;

	std::string d_activeSceneNameDisplay;
};

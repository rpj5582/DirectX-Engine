#pragma once

#include "AntTweakBar\AntTweakBar.h"

#include <string>

class Scene;

class DebugWindow
{
public:
	DebugWindow(std::string windowID, std::string windowLabel);
	virtual ~DebugWindow();

	virtual void setupControls(Scene* scene) = 0;

	void addButton(std::string buttonID, std::string buttonLabel, std::string group, TwButtonCallback callback, void* object, std::string additionalParams = "");
	void addSeparator(std::string separatorID, std::string group, std::string additionalParams = "");

	std::string getWindowID() const;
	std::string getWindowLabel() const;
	TwBar* getWindow() const;

protected:
	std::string m_windowID;
	std::string m_windowLabel;

	TwBar* m_window;
};

void TW_CALL copyStringToClient(std::string& destination, const std::string& source);

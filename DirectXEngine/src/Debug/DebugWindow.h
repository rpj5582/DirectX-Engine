#pragma once

#include "../Debug/DebugGlobals.h"

#include <string>

class DebugWindow
{
public:
	DebugWindow(std::string windowID, std::string windowLabel);
	virtual ~DebugWindow();

	virtual void setupControls() = 0;

	void addButton(std::string buttonID, std::string buttonLabel, std::string group, TwButtonCallback callback, void* object, std::string additionalParams = "");
	void addSeparator(std::string separatorID, std::string group, std::string additionalParams = "");

	void clear();

	std::string getWindowID() const;
	std::string getWindowLabel() const;
	TwBar* getWindow() const;

protected:
	std::string m_windowID;
	std::string m_windowLabel;

	TwBar* m_window;
};

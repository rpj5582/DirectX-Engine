#pragma once

#include "../Third Party/imgui/imgui.h"

#include <string>

enum Choice
{
	NONE,
	OK,
	CANCEL,
	INVALID
};

class IDebugEditor
{
public:
	IDebugEditor();
	~IDebugEditor();

	virtual void draw() = 0;

protected:
	Choice showTextInputPopup(std::string windowName, std::string label, std::string& result);
	std::string getPopupInputText() const;
	void setPopupInputText(const std::string text);

private:
	char m_popupInputTextBuffer[32];
};
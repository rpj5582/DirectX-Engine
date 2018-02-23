#pragma once
#include "IDebugEditor.h"

#include <queue>

class DebugConsoleWindow : public IDebugEditor
{
public:
	DebugConsoleWindow();
	~DebugConsoleWindow();

	void addText(std::string text, unsigned int r = 255, unsigned int g = 255, unsigned int b = 255);

	void draw() override;

private:
	static const unsigned int TEXT_SIZE = 128;
	static const unsigned int HISTORY_SIZE = 1024;

	char m_inputText[TEXT_SIZE];
	std::queue<std::vector<unsigned char>> m_history;

	bool m_shouldScroll;
};
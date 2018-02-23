#include "DebugConsoleWindow.h"

DebugConsoleWindow::DebugConsoleWindow() : m_inputText("")
{
	m_shouldScroll = false;
}

DebugConsoleWindow::~DebugConsoleWindow()
{
}

void DebugConsoleWindow::addText(std::string text, unsigned int r, unsigned int g, unsigned int b)
{
	unsigned char textColored[TEXT_SIZE + 3] = { (unsigned char)r, (unsigned char)g, (unsigned char)b };
	memcpy(textColored + 3, text.c_str(), TEXT_SIZE);

	std::vector<unsigned char> textVec = std::vector<unsigned char>(textColored, textColored + TEXT_SIZE + 3);
	m_history.push(textVec);

	if (m_history.size() > HISTORY_SIZE)
		m_history.pop();

	m_shouldScroll = true;
}

void DebugConsoleWindow::draw()
{
	ImGuiIO& io = ImGui::GetIO();

	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);

	ImGui::Begin("Console", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav);

	ImGui::BeginChild("ConsoleText", ImVec2(io.DisplaySize.x / 2.0f - 315.0f, 225.0f), true);

	for (unsigned int i = 0; i < m_history.size(); i++)
	{
		std::vector<unsigned char> textWithColors = m_history.front();
		std::vector<unsigned char> text = std::vector<unsigned char>(textWithColors.begin() + 3, textWithColors.end());

		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4((float)textWithColors[0] / 255.0f, (float)textWithColors[1] / 255.0f, (float)textWithColors[2] / 255.0f, 1.0f));

		ImGui::Text((char*)text.data());
		m_history.pop();
		m_history.push(textWithColors);

		ImGui::PopStyleColor();
	}

	if (m_shouldScroll)
	{
		ImGui::SetScrollHere();
		m_shouldScroll = false;
	}

	ImGui::EndChild();

	if (ImGui::InputText("##Console Input", m_inputText, TEXT_SIZE, ImGuiInputTextFlags_EnterReturnsTrue))
	{
		addText(m_inputText);
		memset(m_inputText, 0, TEXT_SIZE);
	}
	
	ImGui::End();

	ImGui::PopStyleVar();
}

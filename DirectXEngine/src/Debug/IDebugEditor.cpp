#include "IDebugEditor.h"

#include "Debug.h"

IDebugEditor::IDebugEditor() : m_popupInputTextBuffer("")
{
}

IDebugEditor::~IDebugEditor()
{
}

Choice IDebugEditor::showTextInputPopup(std::string windowName, std::string label, std::string& result)
{
	if (ImGui::BeginPopupModal(windowName.c_str(), nullptr, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::Text(label.c_str());
		ImGui::InputText(("##" + windowName + "InputText").c_str(), m_popupInputTextBuffer, IM_ARRAYSIZE(m_popupInputTextBuffer));

		ImGui::Dummy(ImVec2(0.0f, 5.0f));

		if (ImGui::Button("OK", ImVec2(125.0f, 20.0f)))
		{
			std::string textInputBuffer = std::string(m_popupInputTextBuffer);

			std::string::size_type textBegin = textInputBuffer.find_first_not_of(" \t");
			if (textBegin != std::string::npos)
			{
				std::string::size_type textEnd = textInputBuffer.find_last_not_of(" \t");
				std::string::size_type textLength = textEnd - textBegin + 1;

				result = textInputBuffer.substr(textBegin, textLength).c_str();
				ImGui::CloseCurrentPopup();
				ImGui::EndPopup();
				return OK;
			}
			else
			{
				ImGui::EndPopup();
				return INVALID;
			}
		}

		ImGui::SameLine();

		if (ImGui::Button("Cancel", ImVec2(125.0f, 20.0f)))
		{
			ImGui::CloseCurrentPopup();
			ImGui::EndPopup();
			return CANCEL;
		}

		ImGui::EndPopup();
	}

	return NONE;
}

std::string IDebugEditor::getPopupInputText() const
{
	return m_popupInputTextBuffer;
}

void IDebugEditor::setPopupInputText(const std::string text)
{
	memset(m_popupInputTextBuffer, 0, 32);
	text._Copy_s(m_popupInputTextBuffer, 32, text.size());
}

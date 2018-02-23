#pragma once
#include "IDebugEditor.h"

#include <string>

class Component;
struct DebugComponentData;

class DebugComponentList : public IDebugEditor
{
public:
	DebugComponentList();
	~DebugComponentList();

	void setSelectedEntityID(unsigned int entityID);

	void draw() override;

private:
	bool handleVariable(Component* component, DebugComponentData& componentData);

	template <typename T>
	bool handleAssetVariable(Component* component, DebugComponentData& componentData, const char* payloadType);

	unsigned int m_selectedEntityID;
};

template<typename T>
inline bool DebugComponentList::handleAssetVariable(Component* component, DebugComponentData& componentData, const char* payloadType)
{
	static_assert(std::is_base_of<Asset, T>::value, "Given type is not an Asset.");

	bool varModified = false;

	T* asset;
	if (componentData.getterFunc)
		componentData.getterFunc(component, &asset);
	else
		asset = *static_cast<T**>(componentData.data);

	char assetName[32] = "";
	std::string assetNameStr;
	if (asset)
		assetNameStr = asset->getAssetID();
	else
		assetNameStr = "none";
	assetNameStr._Copy_s(assetName, 32, assetNameStr.size());

	ImGui::InputText(componentData.label.c_str(), assetName, IM_ARRAYSIZE(assetName), ImGuiInputTextFlags_ReadOnly);
	if (ImGui::BeginDragDropTarget())
	{
		const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(payloadType);
		if (payload)
		{
			if (componentData.setterFunc)
				componentData.setterFunc(component, payload->Data);
			else
				*static_cast<T**>(componentData.data) = *static_cast<T*const*>(payload->Data);
		}

		ImGui::EndDragDropTarget();
	}

	ImGui::SameLine();
	if (ImGui::Button(("Clear##" + assetNameStr).c_str()))
	{
		varModified = true;

		T* none = nullptr;
		if (componentData.setterFunc)
			componentData.setterFunc(component, &none);
		else
			*static_cast<T**>(componentData.data) = *static_cast<T**>(&none);
	}

	return varModified;
}

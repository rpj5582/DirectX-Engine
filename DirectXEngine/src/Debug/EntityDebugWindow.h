#pragma once
#include "DebugWindow.h"

#include <unordered_set>

class Entity;
class Component;

class EntityDebugWindow : public DebugWindow
{
public:
	EntityDebugWindow(std::string windowID, std::string windowLabel);
	~EntityDebugWindow();

	void setupControls() override;

	void addEntity(Entity* entity);
	void removeEntity(Entity* entity);

	void addComponent(Component* component);
	void removeComponent(Component* component);

	void addGroup(Component* component, std::string group, std::string additionalParams = "");

	template<typename T>
	void addVariable(T* var, TwType varType, std::string varName, Component* component, std::string group = "", std::string additionalParams = "", bool readonly = false);
	void addVariableWithCallbacks(TwType varType, std::string varName, Component* component, TwGetVarCallback getCallback, TwSetVarCallback setCallback, void* object, std::string group = "", std::string additionalParams = "");

	std::string getEntityDebugName(const Entity* entity, std::string* out_parentDebugName) const;
	std::string getComponentDebugName(const Component* component, std::string* out_entityDebugName) const;

	void showEntity(const Entity* entity) const;
	void hideEntity(const Entity* entity) const;
	void showAllEntities() const;
	void hideAllEntities() const;

private:
	std::unordered_set<std::string> m_entityNames;
};

template<typename T>
inline void EntityDebugWindow::addVariable(T* var, TwType varType, std::string varName, Component* component, std::string group, std::string additionalParams, bool readonly)
{
#if defined(DEBUG) || defined(_DEBUG)
	if (!var || !component) return;

	std::string entityName;
	std::string componentName = getComponentDebugName(component, &entityName);
	std::string spacelessVarName = componentName + removeSpacesFromString(varName);
	std::string spacelessGroupName = componentName + removeSpacesFromString(group);

	if (readonly)
	{
		TwAddVarRO(m_window, spacelessVarName.c_str(), varType, var, "");
	}
	else
	{
		TwAddVarRW(m_window, spacelessVarName.c_str(), varType, var, "");
	}

	std::string description = " " + m_windowID + "/" + spacelessVarName + " group=" + spacelessGroupName + " label='" + varName + "'" + additionalParams;
	TwDefine(description.c_str());

	description = " " + m_windowID + "/" + componentName + " group=" + entityName + " label='" + component->getName() + "' opened=false ";
	TwDefine(description.c_str());
#endif
}

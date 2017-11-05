#include "EntityDebugWindow.h"

#include "../Scene/SceneManager.h"

EntityDebugWindow::EntityDebugWindow(std::string windowID, std::string windowLabel) : DebugWindow(windowID, windowLabel)
{
	m_entityNames = std::unordered_set<std::string>();

	TwDefine((windowID + " position='20 195' size='250 250' ").c_str());
}

EntityDebugWindow::~EntityDebugWindow()
{
}

void EntityDebugWindow::setupControls()
{
#if defined(DEBUG) || defined(_DEBUG)
	Scene* activeScene = SceneManager::getActiveScene();
	if (activeScene)
	{
		addSeparator("AddEntitySeparator", "''");
		TwAddVarRW(m_window, "AddEntityInputField", TW_TYPE_STDSTRING, &activeScene->d_entityNameField, " label='Entity Name' ");
		addButton("AddEntityButton", "Add Entity", "''", &addEntityDebugEditor, activeScene);
		
		addSeparator("MainCameraSeparator", "''");
		TwAddVarCB(m_window, "MainCameraInputField", TW_TYPE_STDSTRING, &setMainCameraSceneDebugEditor, &getMainCameraSceneDebugEditor, activeScene, " label='Main Camera' ");
	}
#endif
}

void EntityDebugWindow::addEntity(Entity* entity)
{
#if defined(DEBUG) || defined(_DEBUG)
	if (!entity) return;

	std::string parentName;
	std::string entityName = getEntityDebugName(entity, &parentName);

	addButton(entityName + "RemoveButton", "Remove Entity", entityName, &removeEntityDebugEditor, entity);
	addSeparator(entityName + "RemoveSeparator", entityName);

	TwAddVarRW(m_window, (entityName + "ChildNameField").c_str(), TW_TYPE_STDSTRING, &entity->d_childNameInputField, (" group='" + entityName + "' label='Child Name' ").c_str());
	addButton(entityName + "AddChildButton", "Add Child", entityName, &addChildEntityDebugEditor, entity);
	addButton(entityName + "RemoveChildButton", "Remove Child", entityName, &removeChildEntityDebugEditor, entity);
	addSeparator(entityName + "ChildSeparator", entityName);

	TwAddVarRW(m_window, (entityName + "ComponentTypeField").c_str(), TW_TYPE_STDSTRING, &entity->d_componentTypeField, (" group='" + entityName + "' label='Component Type' ").c_str());
	addButton(entityName + "AddComponentButton", "Add Component", entityName, &addComponentDebugEditor, entity);
	addSeparator(entityName + "AddComponentSeparator", entityName);

	TwAddVarCB(m_window, (entityName + "Enabled").c_str(), TW_TYPE_BOOLCPP, &setEntityEnabledDebugEditor, &getEntityEnabledDebugEditor, entity, (" label='Enabled' group='" + entityName + "' ").c_str());
	addSeparator(entityName + "EnabledSeparator", entityName);

	std::string description = " " + m_windowID + "/" + entityName + " label='" + entity->getName() + "' opened=false group='" + parentName + "' ";
	TwDefine(description.c_str());

	m_entityNames.insert(entityName);

	std::vector<Component*> components = entity->getAllComponents();
	for (unsigned int i = 0; i < components.size(); i++)
	{
		addComponent(components[i]);
	}

	std::vector<Entity*> children = entity->getChildren();
	for (unsigned int i = 0; i < children.size(); i++)
	{
		addEntity(children[i]);
	}
#endif
}

void EntityDebugWindow::removeEntity(Entity* entity)
{
#if defined(DEBUG) || defined(_DEBUG)
	if (!entity) return;

	std::string entityName = getEntityDebugName(entity, nullptr);
	TwRemoveVar(m_window, entityName.c_str());

	m_entityNames.erase(entityName);
#endif
}

void EntityDebugWindow::addComponent(Component* component)
{
#if defined(DEBUG) || defined(_DEBUG)
	if (!component) return;

	component->initDebugVariables();

	std::string entityName;
	std::string componentName = getComponentDebugName(component, &entityName);

	addSeparator(componentName + "RemoveSeparator", componentName);
	addButton(componentName + "RemoveButton", "Remove Component", componentName, &removeComponentDebugEditor, component);

	TwDefine((" " + m_windowID + "/" + componentName + " label='" + component->getName() + "' group='" + entityName + "' ").c_str());

#endif
}

void EntityDebugWindow::removeComponent(Component* component)
{
#if defined(DEBUG) || defined(_DEBUG)
	if (!component) return;

	std::string componentName = getComponentDebugName(component, nullptr);
	TwRemoveVar(m_window, componentName.c_str());
#endif
}

void EntityDebugWindow::addVariableWithCallbacks(TwType varType, std::string varName, Component* component, TwGetVarCallback getCallback, TwSetVarCallback setCallback, void * object, std::string additionalParams)
{
#if defined(DEBUG) || defined(_DEBUG)
	std::string entityName;
	std::string componentName = getComponentDebugName(component, &entityName);
	std::string spacelessVarName = componentName + removeSpacesFromString(varName);

	TwAddVarCB(m_window, spacelessVarName.c_str(), varType, setCallback, getCallback, object, "");

	std::string description = " " + m_windowID + "/" + spacelessVarName + " group='" + componentName + "' label='" + varName + "' " + additionalParams;
	TwDefine(description.c_str());

	description = " " + m_windowID + "/" + componentName + " group='" + entityName + "' label='" + componentName + "' opened=false ";
	TwDefine(description.c_str());
#endif
}

std::string EntityDebugWindow::getEntityDebugName(const Entity* entity, std::string* out_parentDebugName) const
{
	if (!entity) return "";

	std::string entityName = entity->getName();
	std::string parentName = "";
	const Entity* currentEntity = entity;
	while (true)
	{
		const Entity* parent = currentEntity->getParent();
		if (!parent) break;

		parentName += parent->getName();
		currentEntity = parent;
	}

	std::string spacelessParentName = removeSpacesFromString(parentName);
	std::string spacelessEntityName = removeSpacesFromString(entityName) + spacelessParentName;

	if(out_parentDebugName)
		*out_parentDebugName = spacelessParentName;
	return spacelessEntityName;
}

std::string EntityDebugWindow::getComponentDebugName(const Component* component, std::string* out_entityDebugName) const
{
	if (!component) return "";

	std::string entityName = getEntityDebugName(&component->getEntity(), nullptr);

	std::string componentName = component->getName();
	std::string spacelessComponentName = removeSpacesFromString(componentName);

	if(out_entityDebugName)
		*out_entityDebugName = entityName;
	return entityName + spacelessComponentName;
}

void EntityDebugWindow::showEntity(const Entity* entity) const
{
	std::vector<const Entity*> parentList;
	parentList.push_back(entity);

	Entity* currentParent = entity->getParent();
	while (currentParent)
	{
		parentList.push_back(currentParent);
		currentParent = currentParent->getParent();
	}

	for (auto it = parentList.rbegin(); it != parentList.rend(); it++)
	{
		std::string entityDebugName = getEntityDebugName(*it, nullptr);

		if (m_entityNames.find(entityDebugName) != m_entityNames.end())
		{
			TwDefine((" " + m_windowID + "/" + entityDebugName + " opened=true ").c_str());
		}
	}
}

void EntityDebugWindow::hideEntity(const Entity* entity) const
{
	std::string entityDebugName = getEntityDebugName(entity, nullptr);

	if (m_entityNames.find(entityDebugName) != m_entityNames.end())
	{
		TwDefine((" " + m_windowID + "/" + entityDebugName + " opened=false ").c_str());
	}
}

void EntityDebugWindow::showAllEntities() const
{
	for (auto it = m_entityNames.begin(); it != m_entityNames.end(); it++)
	{
		TwDefine((" " + m_windowID + "/" + *it + " opened=true ").c_str());
	}
}

void EntityDebugWindow::hideAllEntities() const
{
	for (auto it = m_entityNames.begin(); it != m_entityNames.end(); it++)
	{
		TwDefine((" " + m_windowID + "/" + *it + " opened=false ").c_str());
	}
}

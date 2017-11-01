#include "EntityDebugWindow.h"

#include "Debug.h"
#include "../Scene/Scene.h"

EntityDebugWindow::EntityDebugWindow(std::string windowID, std::string windowLabel) : DebugWindow(windowID, windowLabel)
{
	TwDefine((windowID + " position='20 195' size='250 250' ").c_str());
}

EntityDebugWindow::~EntityDebugWindow()
{
}

void EntityDebugWindow::setupControls(Scene* scene)
{
#if defined(DEBUG) || defined(_DEBUG)
	TwAddVarRW(m_window, "AddEntityInputField", TW_TYPE_STDSTRING, &scene->d_entityNameField, " label='Entity Name' ");
	addButton("AddEntityButton", "Add Entity", "''", &addEntityDebugEditor, scene);
	addSeparator("AddEntitySeparator", "");
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

	TwAddVarCB(m_window, (entityName + "Enabled").c_str(), TW_TYPE_BOOLCPP, &setEntityEnabledDebugEditor, &getEntityEnabledDebugEditor, entity, " label='Enabled' ");

	std::string description = " " + m_windowID + "/" + entityName + "Enabled group='" + entityName + "' ";
	TwDefine(description.c_str());

	description = " " + m_windowID + "/" + entityName + " label='" + entity->getName() + "' opened=false group='" + parentName + "' ";
	TwDefine(description.c_str());

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

void TW_CALL addEntityDebugEditor(void* clientData)
{
	Scene* scene = static_cast<Scene*>(clientData);
	if (!scene->d_entityNameField.empty())
		scene->createEntity(scene->d_entityNameField);
	else
		Debug::warning("Could not create entity because no name was given. Please entity a unique name for this entity.");
}

void TW_CALL removeEntityDebugEditor(void* clientData)
{
	Entity* entity = static_cast<Entity*>(clientData);
	entity->getScene().deleteEntity(entity);
}

void TW_CALL addComponentDebugEditor(void* clientData)
{
	Entity* entity = static_cast<Entity*>(clientData);
	entity->addComponentByStringType(entity->d_componentTypeField);
}

void TW_CALL removeComponentDebugEditor(void* clientData)
{
	Component* component = static_cast<Component*>(clientData);
	component->getEntity().removeComponent(component);
	
}

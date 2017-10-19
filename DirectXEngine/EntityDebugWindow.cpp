#include "EntityDebugWindow.h"

#include "Debug.h"
#include "Scene.h"

EntityDebugWindow::EntityDebugWindow(std::string windowID, std::string windowLabel) : DebugWindow(windowID, windowLabel)
{
	TwDefine((windowID + " position='20 150' size='250 250' ").c_str());
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

	std::string entityName = entity->getName();
	std::string spacelessEntityName = removeSpacesFromString(entityName);

	addButton(spacelessEntityName + "RemoveButton", "Remove Entity", spacelessEntityName, &removeEntityDebugEditor, entity);
	addSeparator(spacelessEntityName + "SaveSeparator", spacelessEntityName);

	TwAddVarRW(m_window, (spacelessEntityName + "ComponentTypeField").c_str(), TW_TYPE_STDSTRING, &entity->d_componentTypeField, (" group=" + spacelessEntityName + " label='Component Type' ").c_str());
	addButton(spacelessEntityName + "AddComponentButton", "Add Component", spacelessEntityName, &addComponentDebugEditor, entity);
	addSeparator(spacelessEntityName + "AddComponentSeparator", spacelessEntityName);

	TwAddVarRW(m_window, (spacelessEntityName + "Enabled").c_str(), TW_TYPE_BOOLCPP, &entity->enabled, " label='Enabled' ");

	std::string description = " " + m_windowID + "/" + spacelessEntityName + "Enabled group=" + spacelessEntityName + " ";
	TwDefine(description.c_str());

	description = " " + m_windowID + "/" + spacelessEntityName + " label='" + entityName + "' opened=false ";
	TwDefine(description.c_str());
#endif
}

void EntityDebugWindow::removeEntity(Entity* entity)
{
#if defined(DEBUG) || defined(_DEBUG)
	if (!entity) return;

	std::string entityName = entity->getName();
	std::string spacelessEntityName = removeSpacesFromString(entityName);

	TwRemoveVar(m_window, spacelessEntityName.c_str());
#endif
}

void EntityDebugWindow::addComponent(Component* component)
{
#if defined(DEBUG) || defined(_DEBUG)
	if (!component) return;

	std::string entityName = component->getEntity().getName();
	std::string spacelessEntityName = removeSpacesFromString(entityName);

	std::string componentName = component->getName();
	std::string spacelessComponentName = removeSpacesFromString(componentName);

	std::string componentID = spacelessEntityName + spacelessComponentName;

	addSeparator(componentID + "RemoveSeparator", componentID);
	addButton(componentID + "RemoveButton", "Remove Component", componentID, &removeComponentDebugEditor, component);

	TwDefine((" " + m_windowID + "/" + componentID + " label='" + componentName + "' ").c_str());
#endif
}

void EntityDebugWindow::removeComponent(Component* component)
{
#if defined(DEBUG) || defined(_DEBUG)
	if (!component) return;

	std::string entityName = component->getEntity().getName();
	std::string spacelessEntityName = removeSpacesFromString(entityName);

	std::string componentName = component->getName();
	std::string spacelessComponentName = removeSpacesFromString(componentName);

	TwRemoveVar(m_window, (spacelessEntityName + spacelessComponentName).c_str());
#endif
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

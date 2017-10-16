#include "EntityDebugWindow.h"

#include "Debug.h"
#include "Scene.h"

EntityDebugWindow::EntityDebugWindow(std::string windowID, std::string windowLabel) : DebugWindow(windowID, windowLabel)
{
	TwDefine((windowID + " position='20 250' ").c_str());
}

EntityDebugWindow::~EntityDebugWindow()
{
}

void EntityDebugWindow::addEntity(Entity* entity)
{
#if defined(DEBUG) || defined(_DEBUG)
	if (!entity) return;

	std::string entityName = entity->getName();
	std::string spacelessEntityName = removeSpacesFromString(entityName);

	addButton(spacelessEntityName + "RemoveButton", "Remove Entity", spacelessEntityName, &removeEntityDebugWindow, entity);
	addSeparator(spacelessEntityName + "SaveSeparator", spacelessEntityName);

	TwAddVarRW(m_window, (spacelessEntityName + "ComponentTypeField").c_str(), TW_TYPE_STDSTRING, &entity->d_componentTypeField, (" group=" + spacelessEntityName + " label='Component Type' ").c_str());
	addButton(spacelessEntityName + "AddComponentButton", "Add Component", spacelessEntityName, &addComponentDebugWindow, entity);
	addSeparator(spacelessEntityName + "AddComponentSeparator", spacelessEntityName);

	TwAddVarRW(m_window, (spacelessEntityName + "Enabled").c_str(), TW_TYPE_BOOLCPP, &entity->enabled, " label='Enabled' ");

	std::string description = " " + m_windowID + "/" + spacelessEntityName + "Enabled group=" + spacelessEntityName + " ";
	TwDefine(description.c_str());

	description = " " + m_windowID + "/" + spacelessEntityName + " label='" + entityName + "' opened=false ";
	TwDefine(description.c_str());
#endif
}

void EntityDebugWindow::removeEntity(Scene& scene, Entity* entity)
{
#if defined(DEBUG) || defined(_DEBUG)
	if (!entity) return;

	std::string entityName = entity->getName();
	std::string spacelessEntityName = removeSpacesFromString(entityName);

	TwRemoveVar(m_window, spacelessEntityName.c_str());
	scene.deleteEntity(entity);
#endif
}

void EntityDebugWindow::addComponent(Component* component)
{
	if (!component) return;

	std::string entityName = component->getEntity().getName();
	std::string spacelessEntityName = removeSpacesFromString(entityName);

	std::string componentName = component->getName();
	std::string spacelessComponentName = removeSpacesFromString(componentName);

	std::string componentID = spacelessEntityName + spacelessComponentName;

	addButton(componentID + "RemoveButton", "Remove Component", componentID, &removeComponentDebugWindow, component);
	addSeparator(componentID + "RemoveSeparator", componentID);

	TwDefine((" " + m_windowID + "/" + componentID + " label='" + componentName + "' ").c_str());
}

void EntityDebugWindow::removeComponent(Component* component)
{
	if (!component) return;

	std::string entityName = component->getEntity().getName();
	std::string spacelessEntityName = removeSpacesFromString(entityName);

	std::string componentName = component->getName();
	std::string spacelessComponentName = removeSpacesFromString(componentName);

	TwRemoveVar(m_window, (spacelessEntityName + spacelessComponentName).c_str());
}

void TW_CALL removeEntityDebugWindow(void* clientData)
{
	Entity* entity = static_cast<Entity*>(clientData);
	Debug::entityDebugWindow->removeEntity(entity->getScene(), entity);
}

void TW_CALL addComponentDebugWindow(void* clientData)
{
	Entity* entity = static_cast<Entity*>(clientData);
	entity->addComponentByStringType(entity->d_componentTypeField);
}

void TW_CALL removeComponentDebugWindow(void* clientData)
{
	Component* component = static_cast<Component*>(clientData);
	component->getEntity().removeComponent(component);
	
}

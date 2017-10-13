#pragma once

#include <string>
#include <unordered_map>
#include <typeindex>

#include "CameraComponent.h"
#include "GUIButtonComponent.h"
#include "GUIComponent.h"
#include "GUISpriteComponent.h"
#include "GUITextComponent.h"
#include "GUITransform.h"
#include "LightComponent.h"
#include "MeshRenderComponent.h"
#include "RenderComponent.h"
#include "Transform.h"

#include "FreeCamControls.h"

class Component;
class Entity;

typedef Component*(Entity::*CreateComponentFunc)();

class ComponentRegistry
{
public:
	static Component* addComponentToEntity(Entity& entity, std::string componentType);
	void registerComponents();

	static std::string getTypeName(std::type_index type);

private:
	template<typename T>
	bool registerComponent(std::string componentType);

	void registerEngineComponents();
	void registerCustomComponents();

	static std::unordered_map<std::string, CreateComponentFunc> m_componentRegistry;
	static std::unordered_map<std::type_index, std::string> m_componentRegistryReverse;
};

template<typename T>
inline bool ComponentRegistry::registerComponent(std::string componentType)
{
	static_assert(std::is_base_of<Component, T>::value, "Given type is not a Component.");

	if (m_componentRegistry.find(componentType) == m_componentRegistry.end())
	{
		CreateComponentFunc func = reinterpret_cast<CreateComponentFunc>(&Entity::addComponent<T>);
		m_componentRegistry[componentType] = func;

		m_componentRegistryReverse[typeid(T)] = componentType;
		return true;
	}
	else
	{
		Output::Warning("Did not register component " + componentType + " because it has already been registered.");
		return false;
	}
}

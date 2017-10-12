#pragma once

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

typedef Component*(Entity::*CreateComponentFunc)();

class ComponentRegistry
{
public:
	static Component* addComponentToEntity(Entity& entity, std::string componentType);
	void registerComponents();

private:
	template<typename T>
	bool registerComponent(std::string componentType);

	void registerEngineComponents();
	void registerCustomComponents();

	static std::unordered_map<std::string, CreateComponentFunc> m_componentRegistry;
};

template<typename T>
inline bool ComponentRegistry::registerComponent(std::string componentType)
{
	static_assert(std::is_base_of<Component, T>::value, "Given type is not a Component.");

	if (m_componentRegistry.find(componentType) == m_componentRegistry.end())
	{
		auto test = &Entity::addComponent<T>;
		auto test2 = reinterpret_cast<CreateComponentFunc>(test);
		m_componentRegistry[componentType] = test2;
		return true;
	}
	else
	{
		Output::Warning("Did not register component " + componentType + " because it has already been registered.");
		return false;
	}
}

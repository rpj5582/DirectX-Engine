#include "ComponentRegistry.h"

std::unordered_map<std::string, ComponentRegistry::CreateComponentFunc> ComponentRegistry::m_componentRegistry = std::unordered_map<std::string, CreateComponentFunc>();
std::unordered_map<std::type_index, std::string> ComponentRegistry::m_componentRegistryReverse = std::unordered_map<std::type_index, std::string>();

Component* ComponentRegistry::addComponentToEntity(Entity& entity, std::string componentType)
{
	if (m_componentRegistry.find(componentType) == m_componentRegistry.end())
	{
		Debug::warning("Failed to get component function for component type " + componentType + " because the component was not found in the registry.");
		return false;
	}

	CreateComponentFunc func = m_componentRegistry.at(componentType);
	return (entity.*func)();
}

void ComponentRegistry::registerComponents()
{
	registerEngineComponents();
	registerCustomComponents();
}

std::string ComponentRegistry::getTypeName(std::type_index type)
{
	auto it = m_componentRegistryReverse.find(type);
	if (it != m_componentRegistryReverse.end())
	{
		return it->second;
	}

	Debug::error("Failed to get component type because it wasn't registered. Make sure the component is registered and you're using a fully implemented class!");
	return "";
}

const std::vector<std::string> ComponentRegistry::getAllTypeNames()
{
	std::vector<std::string> typeNames = std::vector<std::string>(m_componentRegistry.size());

	unsigned int i = 0;
	for (auto it = m_componentRegistry.begin(); it != m_componentRegistry.end(); it++)
	{
		typeNames[i] = it->first;
		i++;
	}

	return typeNames;
}

void ComponentRegistry::registerEngineComponents()
{
	// Engine components
	registerComponent<CameraComponent>("Camera");
	registerComponent<GUIButtonComponent>("GUIButtonComponent");
	registerComponent<GUISpriteComponent>("GUISpriteComponent");
	registerComponent<GUIDebugSpriteComponent>("GUIDebugSpriteComponent");
	registerComponent<GUITextComponent>("GUITextComponent");
	registerComponent<GUITransform>("GUITransform");
	registerComponent<LightComponent>("LightComponent");
	registerComponent<MeshRenderComponent>("MeshRenderComponent");
	registerComponent<RenderComponent>("RenderComponent");
	registerComponent<Transform>("Transform");
	registerComponent<FreeCamControls>("FreeCamControls");
}

void ComponentRegistry::registerCustomComponents()
{
	// REGISTER ALL COMPONENTS HERE (keep in alphabetical order for neatness)
}

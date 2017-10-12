#include "ComponentRegistry.h"

std::unordered_map<std::string, CreateComponentFunc> ComponentRegistry::m_componentRegistry = std::unordered_map<std::string, CreateComponentFunc>();

Component* ComponentRegistry::addComponentToEntity(Entity& entity, std::string componentType)
{
	if (m_componentRegistry.find(componentType) == m_componentRegistry.end())
	{
		Output::Warning("Failed to get component function for component type " + componentType + " because the component was not found in the registry.");
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

void ComponentRegistry::registerEngineComponents()
{
	// Engine components
	registerComponent<CameraComponent>("Camera");
	registerComponent<GUIButtonComponent>("GUIButtonComponent");
	registerComponent<GUISpriteComponent>("GUISpriteComponent");
	registerComponent<GUITextComponent>("GUITextComponent");
	registerComponent<GUITransform>("GUITransform");
	registerComponent<LightComponent>("LightComponent");
	registerComponent<MeshRenderComponent>("MeshRenderComponent");
	registerComponent<RenderComponent>("RenderComponent");
	registerComponent<Transform>("Transform");
}

void ComponentRegistry::registerCustomComponents()
{
	// REGISTER ALL COMPONENTS HERE (keep in alphabetical order for neatness)

	registerComponent<FreeCamControls>("FreeCamControls");
}

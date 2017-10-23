#include "Entity.h"

#include "Component/ComponentRegistry.h"

Entity::Entity(Scene& scene, std::string name) : m_scene(scene)
{
	m_name = name;
	m_components = std::vector<Component*>();
	enabled = true;
	d_componentTypeField = "";
}

Entity::~Entity()
{
	for (unsigned int i = 0; i < m_components.size(); i++)
	{
		delete m_components[i];
	}
	m_components.clear();
}

void Entity::update(float deltaTime, float totalTime)
{
	for (unsigned int i = 0; i < m_components.size(); i++)
	{
		if (m_components[i]->enabled)
			m_components[i]->update(deltaTime, totalTime);
	}
}

void Entity::lateUpdate(float deltaTime, float totalTime)
{
	for (unsigned int i = 0; i < m_components.size(); i++)
	{
		if (m_components[i]->enabled)
			m_components[i]->lateUpdate(deltaTime, totalTime);
	}
}

void Entity::saveToJSON(rapidjson::Writer<rapidjson::StringBuffer>& writer) const
{
	writer.StartObject();

	writer.Key("name");
	writer.String(m_name.c_str());

	writer.Key("enabled");
	writer.Bool(enabled);

	writer.Key("components");
	writer.StartArray();

	for (unsigned int i = 0; i < m_components.size(); i++)
	{
		writer.StartObject();

		std::string typeName = ComponentRegistry::getTypeName(typeid(*m_components[i]));
		writer.Key("type");
		writer.Key(typeName.c_str());

		writer.Key("data");
		writer.StartObject();
		m_components[i]->saveToJSON(writer);
		writer.EndObject();
		

		writer.EndObject();
	}

	writer.EndArray();
	writer.EndObject();

	
}

Scene& Entity::getScene() const
{
	return m_scene;
}

std::string Entity::getName() const
{
	return m_name;
}

Component* Entity::addComponentByStringType(std::string componentType)
{
	if (componentType == "")
	{
		Debug::warning("Enter a component type to add.");
		return nullptr;
	}

	return ComponentRegistry::addComponentToEntity(*this, componentType);
}

std::vector<Component*>& Entity::getComponents()
{
	return m_components;
}

void Entity::removeComponent(Component* component)
{
	for (unsigned int i = 0; i < m_components.size(); i++)
	{
		if (m_components[i] == component)
		{
			Debug::entityDebugWindow->removeComponent(component);
			delete component;
			m_components.erase(m_components.begin() + i);
			return;
		}
	}
}

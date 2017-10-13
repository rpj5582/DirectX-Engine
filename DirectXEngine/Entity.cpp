#include "Entity.h"

#include "ComponentRegistry.h"

Entity::Entity(std::string name)
{
	m_name = name;
	m_components = std::vector<Component*>();
	enabled = true;

	m_mouseDownCallbacks = std::vector<Component*>();
	m_mouseUpCallbacks = std::vector<Component*>();
	m_mouseMoveCallbacks = std::vector<Component*>();
	m_mouseWheelCallbacks = std::vector<Component*>();
}

Entity::~Entity()
{
	for (unsigned int i = 0; i < m_components.size(); i++)
	{
		delete m_components[i];
	}
	m_components.clear();

	m_mouseDownCallbacks.clear();
	m_mouseUpCallbacks.clear();
	m_mouseMoveCallbacks.clear();
	m_mouseWheelCallbacks.clear();
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

std::string Entity::getName() const
{
	return m_name;
}

Component* Entity::addComponentByStringType(std::string componentType)
{
	return ComponentRegistry::addComponentToEntity(*this, componentType);
}

std::vector<Component*>& Entity::getComponents()
{
	return m_components;
}

void Entity::subscribeMouseDown(Component* component)
{
	if (component)
		m_mouseDownCallbacks.push_back(component);
}

void Entity::subscribeMouseUp(Component* component)
{
	if (component)
		m_mouseUpCallbacks.push_back(component);
}

void Entity::subscribeMouseMove(Component* component)
{
	if (component)
		m_mouseMoveCallbacks.push_back(component);
}

void Entity::subscribeMouseWheel(Component* component)
{
	if (component)
		m_mouseWheelCallbacks.push_back(component);
}

void Entity::unsubscribeMouseDown(Component* component)
{
	for (unsigned int i = 0; i < m_mouseDownCallbacks.size(); i++)
	{
		if (m_mouseDownCallbacks[i] == component)
		{
			m_mouseDownCallbacks.erase(m_mouseDownCallbacks.begin() + i);
			return;
		}
	}

	Output::Warning("Component not unsubscribed because callback could not be found.");
	return;
}

void Entity::unsubscribeMouseUp(Component* component)
{
	for (unsigned int i = 0; i < m_mouseUpCallbacks.size(); i++)
	{
		if (m_mouseUpCallbacks[i] == component)
		{
			m_mouseUpCallbacks.erase(m_mouseUpCallbacks.begin() + i);
			return;
		}
	}

	Output::Warning("Component not unsubscribed because callback could not be found.");
	return;
}

void Entity::unsubscribeMouseMove(Component* component)
{
	for (unsigned int i = 0; i < m_mouseMoveCallbacks.size(); i++)
	{
		if (m_mouseMoveCallbacks[i] == component)
		{
			m_mouseMoveCallbacks.erase(m_mouseMoveCallbacks.begin() + i);
			return;
		}
	}

	Output::Warning("Component not unsubscribed because callback could not be found.");
	return;
}

void Entity::unsubscribeMouseWheel(Component* component)
{
	for (unsigned int i = 0; i < m_mouseWheelCallbacks.size(); i++)
	{
		if (m_mouseWheelCallbacks[i] == component)
		{
			m_mouseWheelCallbacks.erase(m_mouseWheelCallbacks.begin() + i);
			return;
		}
	}

	Output::Warning("Component not unsubscribed because callback could not be found.");
	return;
}

void Entity::onMouseDown(WPARAM buttonState, int x, int y)
{
	for (unsigned int i = 0; i < m_mouseDownCallbacks.size(); i++)
	{
		m_mouseDownCallbacks[i]->onMouseDown(buttonState, x, y);
	}
}

void Entity::onMouseUp(WPARAM buttonState, int x, int y)
{
	for (unsigned int i = 0; i < m_mouseUpCallbacks.size(); i++)
	{
		m_mouseUpCallbacks[i]->onMouseUp(buttonState, x, y);
	}
}

void Entity::onMouseMove(WPARAM buttonState, int x, int y)
{
	for (unsigned int i = 0; i < m_mouseMoveCallbacks.size(); i++)
	{
		m_mouseMoveCallbacks[i]->onMouseMove(buttonState, x, y);
	}
}

void Entity::onMouseWheel(float wheelDelta, int x, int y)
{
	for (unsigned int i = 0; i < m_mouseWheelCallbacks.size(); i++)
	{
		m_mouseWheelCallbacks[i]->onMouseWheel(wheelDelta, x, y);
	}
}

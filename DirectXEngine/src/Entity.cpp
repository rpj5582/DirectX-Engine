#include "Entity.h"

#include "Component/ComponentRegistry.h"
#include "Scene/Scene.h"

Entity::Entity(Scene& scene, std::string name) : m_scene(scene)
{
	m_name = name;
	m_components = std::vector<Component*>();
	enabled = true;

	m_parent = nullptr;
	m_children = std::vector<Entity*>();

	d_componentTypeField = "";

	d_parentNameInputField = "";
	d_childNameInputField = "";
	d_childrenNames = std::vector<std::string>();
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

std::vector<Component*> Entity::getAllComponents() const
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

Entity* Entity::getParent() const
{
	return m_parent;
}

void Entity::setParent(Entity* parent)
{
	if (this == parent)
	{
		Debug::warning("Parent not assigned to " + m_name + " because the parent given was itself.");
		return;
	}

	if (parent)
	{
		setParentNonRecursive(parent);
		parent->addChildNonRecursive(this);
	}
	else
	{
		m_parent->removeChild(this);
	}
}

Entity* Entity::getChild(unsigned int index) const
{
	if (index >= 0 && index < m_children.size())
		return m_children.at(index);

	Debug::warning("Child index " + std::to_string(index) + " outside bounds of children list for entity " + m_name);
	return nullptr;
}

Entity* Entity::getChildByName(std::string childName) const
{
	for (unsigned int i = 0; i < m_children.size(); i++)
	{
		if (m_children[i]->m_name == childName)
			return m_children[i];
	}

	Debug::warning("Child with name " + childName + " could not be found in child list for entity " + m_name);
	return nullptr;
}

std::vector<Entity*> Entity::getChildren() const
{
	return m_children;
}

void Entity::addChild(Entity* child)
{
	if (!child)
	{
		Debug::warning("Attempted to add null child to entity " + m_name);
		return;
	}

	if (this == child)
	{
		Debug::warning("Child not added to " + m_name + " because the child given was itself.");
		return;
	}

	addChildNonRecursive(child);
	child->setParentNonRecursive(this);
}

void Entity::removeChild(Entity* child)
{
	for (unsigned int i = 0; i < m_children.size(); i++)
	{
		if (m_children[i] == child)
		{
			m_children[i]->setParentNonRecursive(nullptr);
			m_children.erase(m_children.begin() + i);
			return;
		}
	}

	Debug::warning("Child with name " + child->m_name + " could not be found in the child list of entity " + m_name);
}

void Entity::removeChildByIndex(unsigned int index)
{
	if (index >= 0 && index < m_children.size())
	{
		m_children.at(index)->setParentNonRecursive(nullptr);
		m_children.erase(m_children.begin() + index);
		return;
	}

	Debug::warning("Child index " + std::to_string(index) + " outside bounds of children list for entity " + m_name);
}

void Entity::removeChildByName(std::string childName)
{
	for (unsigned int i = 0; i < m_children.size(); i++)
	{
		if (m_children[i]->m_name == childName)
		{
			m_children.at(i)->setParentNonRecursive(nullptr);
			m_children.erase(m_children.begin() + i);
			return;
		}
	}

	Debug::warning("Child with name " + childName + " could not be found in child list for entity " + m_name);
}

void Entity::removeAllChildren()
{
	m_children.clear();
}

void Entity::setParentNonRecursive(Entity* parent)
{
	//Debug::entityDebugWindow->removeEntity(this);
	m_parent = parent;
	//Debug::entityDebugWindow->addEntity(this);
}

void Entity::addChildNonRecursive(Entity* child)
{
	m_children.push_back(child);
}

void TW_CALL setEntityParentNameDebugEditor(void* clientData)
{
	Entity* entity = static_cast<Entity*>(clientData);
	entity->setParent(entity->getScene().getEntityByName(entity->d_parentNameInputField));
}

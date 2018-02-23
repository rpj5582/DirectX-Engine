#include "Entity.h"

#include "Scene/Scene.h"
#include "Component/ComponentRegistry.h"

using namespace DirectX;

Entity::Entity(Scene& scene, unsigned int id, std::string name, bool hasDebugIcon) : m_scene(scene)
{
	m_id = id;
	m_name = name;
	m_components = std::vector<Component*>();
	m_enabled = true;

	m_parent = nullptr;
	m_children = std::vector<Entity*>();

	selected = false;

	m_tags = std::unordered_set<std::string>();

	d_debugIcon = nullptr;

	d_componentTypeField = "";
	d_childNameInputField = "";

#if defined(DEBUG) || defined(_DEBUG)
	if (hasDebugIcon)
	{
		createDebugIcon();
	}
#endif
}

Entity::~Entity()
{
#if defined(DEBUG) || defined(_DEBUG)
	deleteDebugIcon();
#endif

	while (m_components.size() > 0)
	{
		delete m_components.back();
		m_components.pop_back();
	}

	m_parent = nullptr;
	m_children.clear();

	m_tags.clear();
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

void Entity::saveToJSON(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer) const
{
	writer.StartObject();

	writer.Key("name");
	writer.String(m_name.c_str());

	writer.Key("enabled");
	writer.Bool(m_enabled);

	writer.Key("children");
	writer.StartArray();

	for (unsigned int i = 0; i < m_children.size(); i++)
	{
		writer.String(m_children[i]->getName().c_str());
	}

	writer.EndArray();

	writer.Key("tags");
	writer.StartArray();

	for (auto it = m_tags.begin(); it != m_tags.end(); it++)
	{
		writer.String(it->c_str());
	}

	writer.EndArray();

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

void Entity::onSceneLoaded()
{
	for (unsigned int i = 0; i < m_components.size(); i++)
	{
		m_components[i]->onSceneLoaded();
	}
}

Scene& Entity::getScene() const
{
	return m_scene;
}

unsigned int Entity::getID() const
{
	return m_id;
}

std::string Entity::getName() const
{
	return m_name;
}

void Entity::rename(std::string name)
{
	m_name = name;
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
			m_components.erase(m_components.begin() + i);
			delete component;
			return;
		}
	}
}

bool Entity::getEnabled() const
{
	if (!m_enabled) return false;

	const Entity* current = this;
	while (true)
	{
		if (!current->m_parent)
		{
			return true;
		}
		else
		{
			if (current->m_parent->m_enabled)
				current = current->m_parent;
			else
				return false;
		}
	}

	return false;
}

void Entity::setEnabled(bool enabled)
{
	m_enabled = enabled;
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
		if(m_parent)
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

	const Entity* current = this;
	while (true)
	{
		if (!current) break;

		if (current == child)
		{
			Debug::warning("Child " + child->getName() + " not added to entity " + m_name + " because the child was either itself or it is a parent of " + m_name + ".");
			return;
		}

		current = current->m_parent;
	}

	addChildNonRecursive(child);
	child->setParentNonRecursive(this);
}

void Entity::addChildByName(std::string childName)
{
	if (childName == "")
	{
		Debug::warning("Failed to add child to entity " + m_name + ": no child name specified.");
		return;
	}

	Entity* child = m_scene.getEntityByName(childName);
	addChild(child);
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

void Entity::addTag(std::string tag)
{
	m_scene.addTagToEntity(*this, tag);
}

void Entity::removeTag(std::string tag)
{
	m_scene.removeTagFromEntity(*this, tag);
}

bool Entity::hasTag(std::string tag) const
{
	return m_tags.find(tag) != m_tags.end();
}

std::unordered_set<std::string> Entity::getTags() const
{
	return m_tags;
}

void Entity::createDebugIcon()
{
	d_debugIcon = new DebugEntity(*this);
	disableDebugIcon();
}

void Entity::deleteDebugIcon()
{
	if (d_debugIcon)
	{
		delete d_debugIcon;
		d_debugIcon = nullptr;
	}
}

void Entity::enableDebugIcon()
{
	if (d_debugIcon)
	{
		d_debugIcon->enabled = true;
	}
}

void Entity::disableDebugIcon()
{
	if (d_debugIcon)
	{
		d_debugIcon->enabled = false;
	}
}

DebugEntity* Entity::getDebugIcon() const
{
	return d_debugIcon;
}

void Entity::setParentNonRecursive(Entity* parent)
{
	m_parent = parent;

	Transform* transform = getComponent<Transform>();
	if (transform)
	{
		transform->setDirty();
	}
}

void Entity::addChildNonRecursive(Entity* child)
{
	m_children.push_back(child);
}

void Entity::addTagNonResursive(std::string tag)
{
	m_tags.insert(tag);
}

void Entity::removeTagNonRecursive(std::string tag)
{
	m_tags.erase(tag);
}

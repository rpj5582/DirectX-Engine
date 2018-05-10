#pragma once

#include "Asset/AssetManager.h"
#include "Debug/Debug.h"

#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h"

#if defined(DEBUG) || defined(_DEBUG)
#include "DebugEntity.h"
#endif

#include <string>
#include <vector>
#include <unordered_set>
#include <Windows.h>

#define TAG_MAIN_CAMERA "Main Camera"
#define TAG_LIGHT "Light"
#define TAG_GUI "GUI"
#define TAG_COLLIDER "Collider"
#define TAG_PHYSICSBODY "Physics Body"

class Component;

class Entity
{
public:
	friend class Scene;

	void update(float deltaTime, float totalTime);
	void lateUpdate(float deltaTime, float totalTime);

	void saveToJSON(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer) const;

	void onSceneLoaded();

	Scene& getScene() const;

	unsigned int getID() const;
	std::string getName() const;
	void rename(std::string name);

	template<typename T>
	T* addComponent(bool initialize = true);
	Component* addComponentByStringType(std::string componentType, bool initialize = true);

	template<typename T>
	T* getComponent() const;

	template<typename T>
	std::vector<T*> getComponentsByType() const;

	std::vector<Component*> getAllComponents() const;

	template<typename T>
	void removeComponent();
	void removeComponent(Component* component);

	bool getEnabled() const;
	void setEnabled(bool enabled);

	Entity* getParent() const;
	void setParent(Entity* parent);

	Entity* getChild(unsigned int index) const;
	Entity* getChildByName(std::string childName) const;
	std::vector<Entity*> getChildren() const;

	void addChild(Entity* child);
	void addChildByName(std::string childName);
	void removeChild(Entity* child);
	void removeChildByIndex(unsigned int index);
	void removeChildByName(std::string childName);
	void removeAllChildren();

	void addTag(std::string tag);
	void removeTag(std::string tag);
	bool hasTag(std::string tag) const;
	std::unordered_set<std::string> getTags() const;

	bool selected;

#if defined(DEBUG) || defined(_DEBUG)
	void createDebugIcon();
	void deleteDebugIcon();
	void enableDebugIcon();
	void disableDebugIcon();
	DebugEntity* getDebugIcon() const;
#endif

	std::string d_componentTypeField;
	std::string d_childNameInputField;

private:
	Entity(Scene& scene, unsigned int id, std::string name, bool hasDebugIcon);
	~Entity();

	void setParentNonRecursive(Entity* parent);
	void addChildNonRecursive(Entity* child);

	void addTagNonResursive(std::string tag);
	void removeTagNonRecursive(std::string tag);

	Scene& m_scene;

	unsigned int m_id;
	std::string m_name;
	std::vector<Component*> m_components;

	bool m_enabled;

	Entity* m_parent;
	std::vector<Entity*> m_children;

	std::unordered_set<std::string> m_tags;

#if defined(DEBUG) || defined(_DEBUG)
	DebugEntity* d_debugIcon;
#endif
};

template<typename T>
inline T* Entity::addComponent(bool initialize)
{
	static_assert(std::is_base_of<Component, T>::value, "Given type is not a Component.");

	for (unsigned int i = 0; i < m_components.size(); i++)
	{
		// Don't allow more than one of the same type of component on an entity
		T* component = dynamic_cast<T*>(m_components[i]);
		if (component)
		{
			Debug::warning("Did not add component because a component of the same type already exists on entity " + m_name + ".");
			return nullptr;
		}
	}

	T* component = new T(*this);
	m_components.push_back(component);
	component->initDebugVariables();

	if (initialize)
		component->init();

	return component;
}

template<typename T>
inline T* Entity::getComponent() const
{
	static_assert(std::is_base_of<Component, T>::value, "Given type is not a Component.");

	for (unsigned int i = 0; i < m_components.size(); i++)
	{
		T* component = dynamic_cast<T*>(m_components[i]);
		if (component) return component;
	}

	return nullptr;
}

template<typename T>
inline std::vector<T*> Entity::getComponentsByType() const
{
	static_assert(std::is_base_of<Component, T>::value, "Given type is not a Component.");

	std::vector<T*> components;

	for (unsigned int i = 0; i < m_components.size(); i++)
	{
		T* component = dynamic_cast<T*>(m_components[i]);
		if (component) components.push_back(component);
	}

	return components;
}

template<typename T>
inline void Entity::removeComponent()
{
	for (unsigned int i = 0; i < m_components.size(); i++)
	{
		T* component = dynamic_cast<T*>(m_components[i]);
		if (component)
		{
			removeComponent(component);
			return;
		}
	}

	Debug::warning("Given component was not removed because it could not be found on entity " + m_name + ".");
	return;
}

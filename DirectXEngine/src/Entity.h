#pragma once

#include "rapidjson/document.h"
#include "rapidjson/writer.h"

#include "Debug/Debug.h"

#include <string>
#include <vector>
#include <Windows.h>

class Component;

class Entity
{
public:
	friend class Scene;

	void update(float deltaTime, float totalTime);
	void lateUpdate(float deltaTime, float totalTime);

	void saveToJSON(rapidjson::Writer<rapidjson::StringBuffer>& writer) const;

	Scene& getScene() const;

	std::string getName() const;

	template<typename T>
	T* addComponent();
	Component* addComponentByStringType(std::string componentType);

	template<typename T>
	T* getComponent();

	std::vector<Component*>& getComponents();

	template<typename T>
	void removeComponent();
	void removeComponent(Component* component);

	bool enabled;
	
	std::string d_componentTypeField;

private:
	Entity(Scene& scene, std::string name);
	~Entity();

	Scene& m_scene;

	std::string m_name;
	std::vector<Component*> m_components;
};

template<typename T>
inline T* Entity::addComponent()
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
	component->init();
	Debug::entityDebugWindow->addComponent(component);
	return component;
}

template<typename T>
inline T* Entity::getComponent()
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

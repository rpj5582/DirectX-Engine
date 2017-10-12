#pragma once

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

	std::string getName() const;

	Component* addComponentByStringType(std::string componentType);

	template<typename T>
	T* addComponent();

	template<typename T>
	T* getComponent();

	std::vector<Component*>& getComponents();

	template<typename T>
	void removeComponent();

	void subscribeMouseDown(Component* component);
	void subscribeMouseUp(Component* component);
	void subscribeMouseMove(Component* component);
	void subscribeMouseWheel(Component* component);

	void unsubscribeMouseDown(Component* component);
	void unsubscribeMouseUp(Component* component);
	void unsubscribeMouseMove(Component* component);
	void unsubscribeMouseWheel(Component* component);

	void onMouseDown(WPARAM buttonState, int x, int y);
	void onMouseUp(WPARAM buttonState, int x, int y);
	void onMouseMove(WPARAM buttonState, int x, int y);
	void onMouseWheel(float wheelDelta, int x, int y);

	bool enabled;

private:
	Entity(std::string name);
	~Entity();

	std::string m_name;
	std::vector<Component*> m_components;

	std::vector<Component*> m_mouseDownCallbacks;
	std::vector<Component*> m_mouseUpCallbacks;
	std::vector<Component*> m_mouseMoveCallbacks;
	std::vector<Component*> m_mouseWheelCallbacks;
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
			Output::Warning("Did not add component because a component of the same type already exists on entity " + m_name + ".");
			return nullptr;
		}
	}

	T* component = new T(*this);
	m_components.push_back(component);
	component->init();
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
			delete component;
			m_components.erase(m_components.begin() + i);
			return;
		}
	}

	Output::Warning("Given component was not removed because it could not be found on entity " + m_name + ".");
	return;
}

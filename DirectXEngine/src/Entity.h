#pragma once

#include "Asset/AssetManager.h"
#include "Debug/Debug.h"

#include "rapidjson/document.h"
#include "rapidjson/writer.h"

#include <string>
#include <vector>
#include <Windows.h>

#define TAG_LIGHT "light"
#define TAG_GUI "gui"

class Component;
class GUITransform;
class GUIDebugSpriteComponent;

class Entity
{
public:
	friend class Scene;

	void update(float deltaTime, float totalTime);
	void lateUpdate(float deltaTime, float totalTime);

	void saveToJSON(rapidjson::Writer<rapidjson::StringBuffer>& writer) const;

	void onSceneLoaded();

	Scene& getScene() const;

	std::string getName() const;

	template<typename T>
	T* addComponent(bool showInDebugWindow = true);
	Component* addComponentByStringType(std::string componentType);

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

	bool selected;

#if defined(DEBUG) || defined(_DEBUG)
	GUITransform* getDebugIconTransform() const;
	GUIDebugSpriteComponent* getDebugIconSpriteComponent() const;
#endif

	std::string d_componentTypeField;
	std::string d_childNameInputField;

private:
	Entity(Scene& scene, std::string name, bool hasDebugIcon);
	~Entity();

	void setParentNonRecursive(Entity* parent);
	void addChildNonRecursive(Entity* child);

	void addTagNonResursive(std::string tag);
	void removeTagNonRecursive(std::string tag);

	Scene& m_scene;

	std::string m_name;
	std::vector<Component*> m_components;

	bool m_enabled;

	Entity* m_parent;
	std::vector<Entity*> m_children;

	std::unordered_set<std::string> m_tags;

#if defined(DEBUG) || defined(_DEBUG)
	GUITransform* d_guiDebugTransform;
	GUIDebugSpriteComponent* d_guiDebugSpriteComponent;
#endif
};

template<typename T>
inline T* Entity::addComponent(bool showInDebugWindow)
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

	if(showInDebugWindow)
		Debug::entityDebugWindow->addComponent(component);

#if defined(DEBUG) || defined(_DEBUG)
	if (d_guiDebugSpriteComponent)
	{
		CameraComponent* camera = getComponent<CameraComponent>();
		LightComponent* light = getComponent<LightComponent>();

		if (camera)
		{
			d_guiDebugSpriteComponent->setTexture(DEBUG_TEXTURE_CAMERAICON);
		}
		else if (light)
		{
			d_guiDebugSpriteComponent->setTexture(DEBUG_TEXTURE_LIGHTICON);
		}
	}
#endif

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

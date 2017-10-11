#pragma once

#include "Renderer.h"
#include "GUIRenderer.h"
#include "Camera.h"
#include "FreeCamControls.h"

#include <CommonStates.h>
#include <DirectXMath.h>

class Scene
{
public:
	Scene(ID3D11Device* device, ID3D11DeviceContext* context);
	virtual ~Scene();

	virtual bool init();
	void update(float deltaTime, float totalTime);
	void render();

	DirectX::XMMATRIX getProjectionMatrix() const;
	void updateProjectionMatrix(int width, int height, float nearZ, float farZ);

	float getNearZ() const;
	float getFarZ() const;

	void drawInWireframeMode(bool wireframe);

	Entity createEntity();
	void deleteEntity(Entity entity);

	void getAllEntities(Entity** entities, unsigned int* entityCount);

	template<typename T>
	T* addComponentToEntity(Entity entity);

	template<typename T>
	T* getComponentOfEntity(Entity entity);

	std::vector<Component*>* getComponentsOfEntity(Entity entity);

	template<typename T>
	void removeComponentFromEntity(Entity entity);

	Camera* getMainCamera() const;
	void setMainCamera(Camera* camera);

	void subscribeMouseDown(Component* component);
	void unsubscribeMouseDown(Component* component);
	void subscribeMouseUp(Component* component);
	void unsubscribeMouseUp(Component* component);
	void subscribeMouseMove(Component* component);
	void unsubscribeMouseMove(Component* component);
	void subscribeMouseWheel(Component* component);
	void unsubscribeMouseWheel(Component* component);

	void onMouseDown(WPARAM buttonState, int x, int y);
	void onMouseUp(WPARAM buttonState, int x, int y);
	void onMouseMove(WPARAM buttonState, int x, int y);
	void onMouseWheel(float wheelDelta, int x, int y);

private:
	bool loadFromJSON(std::string filepath);

	void renderGeometry(ID3D11BlendState* blendState, ID3D11DepthStencilState* depthStencilState);
	void renderGUI(ID3D11BlendState* blendState, ID3D11DepthStencilState* depthStencilState);

	ID3D11Device* m_device;
	ID3D11DeviceContext* m_context;

	Renderer* m_renderer;
	GUIRenderer* m_guiRenderer;

	ID3D11RasterizerState* m_rasterizerState;
	bool m_prevUseWireframe;

	DirectX::XMFLOAT4X4 m_projectionMatrix;
	float m_near;
	float m_far;

	std::vector<Entity> m_entities;
	std::unordered_map<Entity, std::vector<Component*>> m_components;
	unsigned int m_entityCounter;

	std::vector<LightComponent*> m_lights;
	std::vector<Camera*> m_cameras;
	std::vector<GUIComponent*> m_guis;

	Camera* m_mainCamera;

	std::vector<Component*> m_mouseDownCallbacks;
	std::vector<Component*> m_mouseUpCallbacks;
	std::vector<Component*> m_mouseMoveCallbacks;
	std::vector<Component*> m_mouseWheelCallbacks;
};

template<typename T>
inline T* Scene::addComponentToEntity(Entity entity)
{
	static_assert(std::is_base_of<Component, T>::value, "Given type is not a Component.");

	for (unsigned int i = 0; i < m_entities.size(); i++)
	{
		if (m_entities[i] == entity)
		{
			for (unsigned int j = 0; j < m_components[entity].size(); j++)
			{
				// Don't allow more than one of the same type of component on an entity
				T* component = dynamic_cast<T*>(m_components[entity][j]);
				if (component)
				{
					Output::Warning("Did not add component because a component of the same type already exists on entity " + std::to_string(entity) + ".");
					return nullptr;
				}
			}

			T* component = new T(this, entity);
			component->init();
			m_components[entity].push_back(component);
			return component;
		}
	}

	Output::Warning("Given entity " + std::to_string(entity) + " could not be found.");
	return nullptr;
}

// addComponent overloads for special component types
template<>
inline LightComponent* Scene::addComponentToEntity<LightComponent>(Entity entity)
{
	for (unsigned int i = 0; i < m_entities.size(); i++)
	{
		if (m_entities[i] == entity)
		{
			for (unsigned int j = 0; j < m_components[entity].size(); j++)
			{
				// Don't allow more than one of the same type of component on an entity
				LightComponent* component = dynamic_cast<LightComponent*>(m_components[entity][j]);
				if (component)
				{
					Output::Warning("Did not add component because a component of the same type already exists on entity " + std::to_string(entity) + ".");
					return nullptr;
				}
			}

			LightComponent* component = new LightComponent(this, entity);
			component->init();
			m_components[entity].push_back(component);
			m_lights.push_back(component); // Adds light components to list of lights
			return component;
		}
	}

	Output::Warning("Given entity " + std::to_string(entity) + " could not be found.");
	return nullptr;
}

template<>
inline Camera* Scene::addComponentToEntity<Camera>(Entity entity)
{
	for (unsigned int i = 0; i < m_entities.size(); i++)
	{
		if (m_entities[i] == entity)
		{
			for (unsigned int j = 0; j < m_components[entity].size(); j++)
			{
				// Don't allow more than one of the same type of component on an entity
				Camera* component = dynamic_cast<Camera*>(m_components[entity][j]);
				if (component)
				{
					Output::Warning("Did not add component because a component of the same type already exists on entity " + std::to_string(entity) + ".");
					return nullptr;
				}
			}

			Camera* component = new Camera(this, entity);
			component->init();
			m_components[entity].push_back(component);
			m_cameras.push_back(component); // Adds camera components to list of cameras
			return component;
		}
	}

	Output::Warning("Given entity " + std::to_string(entity) + " could not be found.");
	return nullptr;
}

template<>
inline GUISpriteComponent* Scene::addComponentToEntity<GUISpriteComponent>(Entity entity)
{
	for (unsigned int i = 0; i < m_entities.size(); i++)
	{
		if (m_entities[i] == entity)
		{
			for (unsigned int j = 0; j < m_components[entity].size(); j++)
			{
				// Don't allow more than one of the same type of component on an entity
				GUISpriteComponent* component = dynamic_cast<GUISpriteComponent*>(m_components[entity][j]);
				if (component)
				{
					Output::Warning("Did not add component because a component of the same type already exists on entity " + std::to_string(entity) + ".");
					return nullptr;
				}
			}

			GUISpriteComponent* component = new GUISpriteComponent(this, entity);
			component->init();
			m_components[entity].push_back(component);
			m_guis.push_back(component); // Adds gui components to list of guis
			return component;
		}
	}

	Output::Warning("Given entity " + std::to_string(entity) + " could not be found.");
	return nullptr;
}

template<>
inline GUITextComponent* Scene::addComponentToEntity<GUITextComponent>(Entity entity)
{
	for (unsigned int i = 0; i < m_entities.size(); i++)
	{
		if (m_entities[i] == entity)
		{
			for (unsigned int j = 0; j < m_components[entity].size(); j++)
			{
				// Don't allow more than one of the same type of component on an entity
				GUITextComponent* component = dynamic_cast<GUITextComponent*>(m_components[entity][j]);
				if (component)
				{
					Output::Warning("Did not add component because a component of the same type already exists on entity " + std::to_string(entity) + ".");
					return nullptr;
				}
			}

			GUITextComponent* component = new GUITextComponent(this, entity);
			component->init();
			m_components[entity].push_back(component);
			m_guis.push_back(component); // Adds gui components to list of guis
			return component;
		}
	}

	Output::Warning("Given entity " + std::to_string(entity) + " could not be found.");
	return nullptr;
}

template<>
inline GUIButtonComponent* Scene::addComponentToEntity<GUIButtonComponent>(Entity entity)
{
	for (unsigned int i = 0; i < m_entities.size(); i++)
	{
		if (m_entities[i] == entity)
		{
			for (unsigned int j = 0; j < m_components[entity].size(); j++)
			{
				// Don't allow more than one of the same type of component on an entity
				GUIButtonComponent* component = dynamic_cast<GUIButtonComponent*>(m_components[entity][j]);
				if (component)
				{
					Output::Warning("Did not add component because a component of the same type already exists on entity " + std::to_string(entity) + ".");
					return nullptr;
				}
			}

			GUIButtonComponent* component = new GUIButtonComponent(this, entity);
			component->init();
			m_components[entity].push_back(component);
			m_guis.push_back(component); // Adds gui components to list of guis
			return component;
		}
	}

	Output::Warning("Given entity " + std::to_string(entity) + " could not be found.");
	return nullptr;
}

template<typename T>
inline T* Scene::getComponentOfEntity(Entity entity)
{
	static_assert(std::is_base_of<Component, T>::value, "Given type is not a Component.");

	for (unsigned int i = 0; i < m_entities.size(); i++)
	{
		if (m_entities[i] == entity)
		{
			for (unsigned int j = 0; j < m_components[entity].size(); j++)
			{
				T* component = dynamic_cast<T*>(m_components[entity][j]);
				if(component) return component;
			}

			return nullptr;
		}
	}

	Output::Warning("Given entity " + std::to_string(entity) + " could not be found.");
	return nullptr;
}

template<typename T>
inline void Scene::removeComponentFromEntity(Entity entity)
{
	static_assert(std::is_base_of<Component, T>::value, "Given type is not a Component.");

	for (unsigned int i = 0; i < m_entities.size(); i++)
	{
		if (m_entities[i] == entity)
		{
			for (unsigned int j = 0; j < m_components[entity].size(); j++)
			{
				T* component = dynamic_cast<T*>(m_components[entity][j]);
				if (component)
				{
					delete m_components[entity][j];
					m_components[entity].erase(m_components[entity].begin() + j);
					return;
				}
			}

			Output::Warning("Given component was not removed because it could not be found on entity " + std::to_string(entity) + ".");
			return;
		}
	}

	Output::Warning("Given entity " + std::to_string(entity) + " could not be found.");
	return;
}

// removeComponent overloads for special component types
template<>
inline void Scene::removeComponentFromEntity<LightComponent>(Entity entity)
{
	for (unsigned int i = 0; i < m_entities.size(); i++)
	{
		if (m_entities[i] == entity)
		{
			for (unsigned int j = 0; j < m_components[entity].size(); j++)
			{
				LightComponent* component = dynamic_cast<LightComponent*>(m_components[entity][j]);
				if (component)
				{
					delete component;
					m_components[entity].erase(m_components[entity].begin() + j);
					
					// component is a pointer to deleted memory. We need to check if we have another 
					// pointer in the list (which we should) and remove that pointer as well.
					for (unsigned int k = 0; k < m_lights.size(); k++)
					{
						if (m_lights[k] == component)
						{
							m_lights.erase(m_lights.begin() + k);
							return;
						}
					}

					return;
				}
			}

			Output::Warning("Given component was not removed because it could not be found on entity " + std::to_string(entity) + ".");
			return;
		}
	}

	Output::Warning("Given entity " + std::to_string(entity) + " could not be found.");
	return;
}

template<>
inline void Scene::removeComponentFromEntity<Camera>(Entity entity)
{
	for (unsigned int i = 0; i < m_entities.size(); i++)
	{
		if (m_entities[i] == entity)
		{
			for (unsigned int j = 0; j < m_components[entity].size(); j++)
			{
				Camera* component = dynamic_cast<Camera*>(m_components[entity][j]);
				if (component)
				{
					delete component;
					m_components[entity].erase(m_components[entity].begin() + j);

					// component is a pointer to deleted memory. We need to check if we have another 
					// pointer in the list (which we should) and remove that pointer as well.
					for (unsigned int k = 0; k < m_cameras.size(); k++)
					{
						if (m_cameras[k] == component)
						{
							m_cameras.erase(m_cameras.begin() + k);
							return;
						}
					}

					return;
				}
			}

			Output::Warning("Given component was not removed because it could not be found on entity " + std::to_string(entity) + ".");
			return;
		}
	}

	Output::Warning("Given entity " + std::to_string(entity) + " could not be found.");
	return;
}

template<>
inline void Scene::removeComponentFromEntity<GUISpriteComponent>(Entity entity)
{
	for (unsigned int i = 0; i < m_entities.size(); i++)
	{
		if (m_entities[i] == entity)
		{
			for (unsigned int j = 0; j < m_components[entity].size(); j++)
			{
				GUISpriteComponent* component = dynamic_cast<GUISpriteComponent*>(m_components[entity][j]);
				if (component)
				{
					delete component;
					m_components[entity].erase(m_components[entity].begin() + j);

					// component is a pointer to deleted memory. We need to check if we have another 
					// pointer in the list (which we should) and remove that pointer as well.
					for (unsigned int k = 0; k < m_guis.size(); k++)
					{
						if (m_guis[k] == component)
						{
							m_guis.erase(m_guis.begin() + k);
							return;
						}
					}

					return;
				}
			}

			Output::Warning("Given component was not removed because it could not be found on entity " + std::to_string(entity) + ".");
			return;
		}
	}

	Output::Warning("Given entity " + std::to_string(entity) + " could not be found.");
	return;
}

template<>
inline void Scene::removeComponentFromEntity<GUITextComponent>(Entity entity)
{
	for (unsigned int i = 0; i < m_entities.size(); i++)
	{
		if (m_entities[i] == entity)
		{
			for (unsigned int j = 0; j < m_components[entity].size(); j++)
			{
				GUITextComponent* component = dynamic_cast<GUITextComponent*>(m_components[entity][j]);
				if (component)
				{
					delete component;
					m_components[entity].erase(m_components[entity].begin() + j);

					// component is a pointer to deleted memory. We need to check if we have another 
					// pointer in the list (which we should) and remove that pointer as well.
					for (unsigned int k = 0; k < m_guis.size(); k++)
					{
						if (m_guis[k] == component)
						{
							m_guis.erase(m_guis.begin() + k);
							return;
						}
					}

					return;
				}
			}

			Output::Warning("Given component was not removed because it could not be found on entity " + std::to_string(entity) + ".");
			return;
		}
	}

	Output::Warning("Given entity " + std::to_string(entity) + " could not be found.");
	return;
}

template<>
inline void Scene::removeComponentFromEntity<GUIButtonComponent>(Entity entity)
{
	for (unsigned int i = 0; i < m_entities.size(); i++)
	{
		if (m_entities[i] == entity)
		{
			for (unsigned int j = 0; j < m_components[entity].size(); j++)
			{
				GUIButtonComponent* component = dynamic_cast<GUIButtonComponent*>(m_components[entity][j]);
				if (component)
				{
					delete component;
					m_components[entity].erase(m_components[entity].begin() + j);

					// component is a pointer to deleted memory. We need to check if we have another 
					// pointer in the list (which we should) and remove that pointer as well.
					for (unsigned int k = 0; k < m_guis.size(); k++)
					{
						if (m_guis[k] == component)
						{
							m_guis.erase(m_guis.begin() + k);
							return;
						}
					}

					return;
				}
			}

			Output::Warning("Given component was not removed because it could not be found on entity " + std::to_string(entity) + ".");
			return;
		}
	}

	Output::Warning("Given entity " + std::to_string(entity) + " could not be found.");
	return;
}
#pragma once

#include "../Entity.h"

#include "../Component/ICollider.h"
#include "../Render/Renderer.h"
#include "../Render/GUIRenderer.h"

#include <DirectXMath.h>

class Scene
{
public:
	friend class SceneManager;

	Scene();
	~Scene();

	bool init();
	void update(float deltaTime, float totalTime);

	void renderGeometry(Renderer* renderer, ID3D11RenderTargetView* backBufferRTV, ID3D11DepthStencilView* backBufferDSV, float width, float height);
	void renderGUI(GUIRenderer* guiRenderer);

	Entity* createEntity(std::string name);
	void deleteEntity(Entity* entity);

	Entity* getEntityByName(std::string name);
	Entity* getEntityWithTag(std::string tag);
	std::vector<Entity*> getAllEntities() const;
	std::vector<Entity*> getAllEntitiesWithTag(std::string tag) const;

	template<typename T>
	std::vector<T*> getAllComponentsByType() const;

	void addTag(std::string tag);
	std::vector<std::string> getAllTags() const;

	void addTagToEntity(Entity& entity, std::string tag);
	void removeTagFromEntity(Entity& entity, std::string tag);

	bool isDirty() const;

	CameraComponent* getMainCamera() const;
	void setMainCamera(CameraComponent* camera);
	void setMainCamera(Entity* entity);

	CameraComponent* getDebugCamera() const;

private:
	bool hasFilePath() const;

	bool loadFromJSON(std::string filepath);

	void saveToJSON();
	void saveToJSON(std::string filepath);

	unsigned int getEntityIndex(const Entity& entity) const;

	std::string m_filepath;
	bool m_dirty;

	std::vector<Entity*> m_entities;
	unsigned int m_entityCount;

	std::unordered_map<std::string, std::vector<Entity*>> m_taggedEntities;

	Entity* m_debugCamera;
	CameraComponent* m_mainCamera;
};

template<typename T>
inline std::vector<T*> Scene::getAllComponentsByType() const
{
	static_assert(std::is_base_of<Component, T>::value, "Given type is not a Component.");

	std::vector<T*> components;

	for (unsigned int i = 0; i < m_entities.size(); i++)
	{
		std::vector<T*> entityComponents = m_entities[i]->getComponentsByType<T>();
		components.insert(components.end(), entityComponents.begin(), entityComponents.end());
	}

	return components;
}

#pragma once

#include "../Entity.h"

#include "../Render/Renderer.h"
#include "../Render/GUIRenderer.h"

#include <DirectXMath.h>

class Scene
{
public:
	friend class SceneManager;

	Scene(ID3D11Device* device, ID3D11DeviceContext* context, std::string name, std::string filepath);
	virtual ~Scene();

	virtual bool init();
	virtual void update(float deltaTime, float totalTime);
	void render();

	std::string getName() const;

	Entity* createEntity(std::string name);
	void deleteEntity(Entity* entity);

	void clear();

	Entity* getEntityByName(std::string name);
	Entity* getEntityWithTag(std::string tag);
	std::vector<Entity*> getAllEntities() const;
	std::vector<Entity*> getAllEntitiesWithTag(std::string tag) const;

	template<typename T>
	std::vector<T*> getAllComponentsByType() const;

	void addTag(std::string tag);

	void addTagToEntity(Entity& entity, std::string tag);
	void removeTagFromEntity(Entity& entity, std::string tag);

	CameraComponent* getMainCamera() const;
	void setMainCamera(CameraComponent* camera);
	void setMainCamera(Entity* entity);

	CameraComponent* getDebugCamera() const;

	std::string d_entityNameField;
	std::string d_mainCameraField;

	std::string d_textureNameField;
	std::string d_materialNameField;
	std::string d_meshNameField;
	std::string d_fontNameField;
	std::string d_samplerNameField;
	std::string d_vertexShaderNameField;
	std::string d_pixelShaderNameField;

	std::string d_texturePathField;
	std::string d_materialPathField;
	std::string d_meshPathField;
	std::string d_fontPathField;
	std::string d_samplerPathField;
	std::string d_vertexShaderPathField;
	std::string d_pixelShaderPathField;

protected:
	virtual void onLoad() = 0;

private:
	bool loadFromJSON();
	void saveToJSON();

	void renderGeometry();
	void renderGUI();

	unsigned int getEntityIndex(const Entity& entity) const;

	ID3D11Device* m_device;
	ID3D11DeviceContext* m_context;

	std::string m_name;
	std::string m_filepath;

	Renderer* m_renderer;
	GUIRenderer* m_guiRenderer;

	ID3D11BlendState* m_blendState;
	ID3D11DepthStencilState* m_depthStencilStateDefault;
	ID3D11DepthStencilState* m_depthStencilStateRead;

	std::vector<Entity*> m_entities;

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

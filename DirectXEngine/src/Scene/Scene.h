#pragma once

#include "../Entity.h"

#include "../Component/CameraComponent.h"
#include "../Component/FreeCamControls.h"

#include "../Render/Renderer.h"
#include "../Render/GUIRenderer.h"

#include <DirectXMath.h>

class Scene
{
public:
	friend class SceneManager;

	Scene(ID3D11Device* device, ID3D11DeviceContext* context, std::string name, std::string filepath, unsigned int windowWidth, unsigned int windowHeight, float nearZ, float farZ);
	virtual ~Scene();

	virtual bool init();
	virtual void update(float deltaTime, float totalTime);
	void render();

	std::string getName() const;

	Entity* createEntity(std::string name);
	void deleteEntity(Entity* entity);

	void clear();

	DirectX::XMMATRIX getProjectionMatrix() const;
	void updateProjectionMatrix(unsigned int windowWidth, unsigned int windowHeight, float nearZ, float farZ);

	float getNearZ() const;
	float getFarZ() const;

	Entity* getEntityByName(std::string name);

	template<typename T>
	std::vector<T*> getAllComponentsByType() const;

	CameraComponent* getMainCamera() const;

	std::string d_entityNameField;

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
	void setMainCamera(CameraComponent* camera);

	virtual void onLoad() = 0;

private:
	bool loadFromJSON();
	void saveToJSON();

	void renderGeometry();
	void renderGUI();

	ID3D11Device* m_device;
	ID3D11DeviceContext* m_context;

	std::string m_name;
	std::string m_filepath;

	Renderer* m_renderer;
	GUIRenderer* m_guiRenderer;

	ID3D11BlendState* m_blendState;
	ID3D11DepthStencilState* m_depthStencilStateDefault;
	ID3D11DepthStencilState* m_depthStencilStateRead;

	DirectX::XMFLOAT4X4 m_projectionMatrix;
	unsigned int m_windowWidth;
	unsigned int m_windowHeight;
	float m_near;
	float m_far;

	std::vector<Entity*> m_entities;

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

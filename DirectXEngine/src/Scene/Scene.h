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
	Scene(ID3D11Device* device, ID3D11DeviceContext* context);
	virtual ~Scene();

	virtual bool init();
	void update(float deltaTime, float totalTime);
	void render();

	bool loadFromJSON(std::string filename);
	void saveToJSON(std::string filename);

	Entity* createEntity(std::string name);
	void deleteEntity(Entity* entity);

	void clear();

	DirectX::XMMATRIX getProjectionMatrix() const;
	void updateProjectionMatrix(int width, int height, float nearZ, float farZ);

	float getNearZ() const;
	float getFarZ() const;

	CameraComponent* getMainCamera() const;

	std::string d_sceneNameField;
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
	Entity* getEntityByName(std::string name);

	void setMainCamera(CameraComponent* camera);

private:
	void renderGeometry();
	void renderGUI();

	ID3D11Device* m_device;
	ID3D11DeviceContext* m_context;

	Renderer* m_renderer;
	GUIRenderer* m_guiRenderer;

	ID3D11BlendState* m_blendState;
	ID3D11DepthStencilState* m_depthStencilStateDefault;
	ID3D11DepthStencilState* m_depthStencilStateRead;

	DirectX::XMFLOAT4X4 m_projectionMatrix;
	float m_near;
	float m_far;

	std::vector<Entity*> m_entities;

	Entity* m_debugCamera;
	CameraComponent* m_mainCamera;
};



#pragma once

#include "Entity.h"

// Temp, should go away when tagging is implemented
#include "CameraComponent.h"

#include "Renderer.h"
#include "GUIRenderer.h"

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

	void onMouseDown(WPARAM buttonState, int x, int y);
	void onMouseUp(WPARAM buttonState, int x, int y);
	void onMouseMove(WPARAM buttonState, int x, int y);
	void onMouseWheel(float wheelDelta, int x, int y);

	std::string d_sceneNameField;
	std::string d_entityNameField;

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

	CameraComponent* m_mainCamera;
};



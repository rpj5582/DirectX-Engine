#pragma once

#include "Entity.h"

// Temp, should go away when tagging is implemented
#include "CameraComponent.h"

#include "Renderer.h"
#include "GUIRenderer.h"

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

	void getAllEntities(Entity*** entities, unsigned int* entityCount);

	CameraComponent* getMainCamera() const;

	void onMouseDown(WPARAM buttonState, int x, int y);
	void onMouseUp(WPARAM buttonState, int x, int y);
	void onMouseMove(WPARAM buttonState, int x, int y);
	void onMouseWheel(float wheelDelta, int x, int y);

protected:
	bool loadFromJSON(std::string filepath);

	Entity* createEntity(std::string name = "Entity");
	void deleteEntity(Entity* entity);

	Entity* getEntityByName(std::string name);

	void setMainCamera(CameraComponent* camera);

private:
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

	std::vector<Entity*> m_entities;

	CameraComponent* m_mainCamera;
};

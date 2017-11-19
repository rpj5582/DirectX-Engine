#pragma once
#include "Component.h"

class CameraComponent : public Component
{
public:
	CameraComponent(Entity& entity);
	~CameraComponent();

	virtual void init() override;
	virtual void lateUpdate(float deltaTime, float totalTime) override;

	DirectX::XMFLOAT4X4 getViewMatrix() const;

	DirectX::XMFLOAT3 screenToWorld(DirectX::XMFLOAT2 screenPoint);
	DirectX::XMFLOAT2 worldToScreen(DirectX::XMFLOAT3 worldPoint);
	bool isVisible(DirectX::XMFLOAT3 worldPoint);

private:
	void updateViewMatrix();

	DirectX::XMFLOAT4X4 m_viewMatrix;
};
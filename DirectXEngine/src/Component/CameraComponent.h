#pragma once
#include "Component.h"

class CameraComponent : public Component
{
public:
	CameraComponent(Entity& entity);
	~CameraComponent();

	virtual void lateUpdate(float deltaTime, float totalTime) override;

	DirectX::XMMATRIX getViewMatrix() const;

private:
	void updateViewMatrix();

	DirectX::XMFLOAT4X4 m_viewMatrix;
};
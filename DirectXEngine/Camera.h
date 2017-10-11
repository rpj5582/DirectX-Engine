#pragma once
#include "Component.h"

#include "Transform.h"

class Camera : public Component
{
public:
	Camera(Scene* scene, Entity entity);
	~Camera();

	DirectX::XMMATRIX getViewMatrix() const;
	void updateViewMatrix();

private:
	DirectX::XMFLOAT4X4 m_viewMatrix;
};
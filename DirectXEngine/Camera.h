#pragma once
#include "Component.h"

#include "Transform.h"

class Camera : public Component
{
public:
	Camera(Scene* scene, unsigned int entity);
	~Camera();

	virtual void init() override;
	virtual void update(float deltaTime, float totalTime) override;

	DirectX::XMMATRIX getViewMatrix() const;
	void updateViewMatrix();

private:
	DirectX::XMFLOAT4X4 m_viewMatrix;
};
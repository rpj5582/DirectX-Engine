#include "Camera.h"

#include "Scene.h"

using namespace DirectX;

Camera::Camera(Scene* scene, Entity entity) : Component(scene, entity)
{
	XMStoreFloat4x4(&m_viewMatrix, XMMatrixIdentity());
}

Camera::~Camera()
{
}

DirectX::XMMATRIX Camera::getViewMatrix() const
{
	return XMLoadFloat4x4(&m_viewMatrix);
}

void Camera::updateViewMatrix()
{
	Transform* transform = scene->getComponentOfEntity<Transform>(entity);
	if (!transform) return;

	XMFLOAT3 cameraPosition = transform->getPosition();

	XMFLOAT3 forwardFloat3 = transform->getForward();
	XMFLOAT3 upFloat3 = transform->getUp();

	XMVECTOR eye = XMLoadFloat3(&cameraPosition);
	XMVECTOR forward = XMLoadFloat3(&forwardFloat3);
	XMVECTOR up = XMLoadFloat3(&upFloat3);

	XMMATRIX viewMatrix = XMMatrixLookToLH(eye, forward, up);
	XMStoreFloat4x4(&m_viewMatrix, viewMatrix);
}

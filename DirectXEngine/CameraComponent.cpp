#include "CameraComponent.h"

#include "Transform.h"

using namespace DirectX;

CameraComponent::CameraComponent(Entity& entity) : Component(entity)
{
	XMStoreFloat4x4(&m_viewMatrix, XMMatrixIdentity());
}

CameraComponent::~CameraComponent()
{
}

void CameraComponent::lateUpdate(float deltaTime, float totalTime)
{
	updateViewMatrix();
}

DirectX::XMMATRIX CameraComponent::getViewMatrix() const
{
	return XMLoadFloat4x4(&m_viewMatrix);
}

void CameraComponent::updateViewMatrix()
{
	Transform* transform = entity.getComponent<Transform>();
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

#include "CameraComponent.h"

#include "Transform.h"
#include "GUIDebugSpriteComponent.h"

using namespace DirectX;

CameraComponent::CameraComponent(Entity& entity) : Component(entity)
{
	XMStoreFloat4x4(&m_viewMatrix, XMMatrixIdentity());
}

CameraComponent::~CameraComponent()
{
}

void CameraComponent::init()
{
	Component::init();

#if defined(DEBUG) || defined(_DEBUG)
	DebugEntity* debugIcon = entity.getDebugIcon();
	if (debugIcon)
	{
		debugIcon->getGUIDebugSpriteComponent()->setTexture(DEBUG_TEXTURE_CAMERAICON);
	}
#endif
}

void CameraComponent::lateUpdate(float deltaTime, float totalTime)
{
	updateViewMatrix();
}

DirectX::XMFLOAT4X4 CameraComponent::getViewMatrix() const
{
	return m_viewMatrix;
}

DirectX::XMFLOAT3 CameraComponent::screenToWorld(DirectX::XMFLOAT2 screenPoint)
{
	XMFLOAT2 adjustedScreenPoint = XMFLOAT2(screenPoint.x, screenPoint.y);
	adjustedScreenPoint.x = 2.0f * adjustedScreenPoint.x / Window::getWidth() - 1;
	adjustedScreenPoint.y = -2.0f * adjustedScreenPoint.y / Window::getHeight() + 1;

	XMVECTOR screenPointVec = XMLoadFloat2(&adjustedScreenPoint);

	XMFLOAT4X4 proj = Window::getProjectionMatrix();
	const XMMATRIX& projectionMatrix = XMLoadFloat4x4(&proj);

	const XMMATRIX& viewMatrix = XMLoadFloat4x4(&m_viewMatrix);

	XMMATRIX inverseViewProjectionMatrix = XMMatrixInverse(nullptr, XMMatrixMultiply(viewMatrix, projectionMatrix));
	XMVECTOR worldPointVec = XMVector3TransformCoord(screenPointVec, inverseViewProjectionMatrix);

	XMFLOAT3 worldPoint;
	XMStoreFloat3(&worldPoint, worldPointVec);

	return worldPoint;
}

DirectX::XMFLOAT2 CameraComponent::worldToScreen(DirectX::XMFLOAT3 worldPoint)
{
	XMVECTOR worldPointVec = XMLoadFloat3(&worldPoint);

	XMFLOAT4X4 proj = Window::getProjectionMatrix();
	XMMATRIX projectionMatrix = XMLoadFloat4x4(&proj);

	XMMATRIX viewMatrix = XMLoadFloat4x4(&m_viewMatrix);

	XMMATRIX viewProjectionMatrix = XMMatrixMultiply(viewMatrix, projectionMatrix);
	XMVECTOR screenPointVec = XMVector3TransformCoord(worldPointVec, viewProjectionMatrix);

	XMFLOAT2 screenPoint;
	XMStoreFloat2(&screenPoint, screenPointVec);

	screenPoint.x = (0.5f + screenPoint.x * 0.5f) * Window::getWidth();
	screenPoint.y = (0.5f - screenPoint.y * 0.5f) * Window::getHeight();

	return screenPoint;
}

bool CameraComponent::isVisible(DirectX::XMFLOAT3 worldPoint)
{
	XMFLOAT4 worldPointFloat4 = XMFLOAT4(worldPoint.x, worldPoint.y, worldPoint.z, 1);
	XMVECTOR worldPointVec = XMLoadFloat4(&worldPointFloat4);

	XMFLOAT4X4 proj = Window::getProjectionMatrix();
	XMMATRIX projectionMatrix = XMLoadFloat4x4(&proj);

	XMMATRIX viewMatrix = XMLoadFloat4x4(&m_viewMatrix);

	XMMATRIX viewProjectionMatrix = XMMatrixMultiply(viewMatrix, projectionMatrix);
	XMVECTOR screenPointVec = XMVector3TransformCoord(worldPointVec, viewProjectionMatrix);

	XMFLOAT4 screenPoint;
	XMStoreFloat4(&screenPoint, screenPointVec);

	if ( screenPoint.x < -1.0f || screenPoint.x > 1.0f || screenPoint.y < -1.0f || screenPoint.y > 1.0f || screenPoint.z < 0.0f || screenPoint.z > 1.0f)
	{
		return false;
	}

	return true;
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

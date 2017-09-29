#include "CameraManager.h"

using namespace DirectX;

CameraManager::CameraManager()
{
	for (unsigned int i = 0; i < MAX_CAMERAS; i++)
	{
		m_cameras[i] = nullptr;
		XMStoreFloat4x4(&m_viewMatrices[i], XMMatrixIdentity());
	}

	XMStoreFloat4x4(&m_projectionMatrix, XMMatrixIdentity());
}

CameraManager::~CameraManager()
{
}

void CameraManager::updateViewMatrices()
{
	// Create the View matrix
	// - In an actual game, recreate this matrix every time the camera 
	//    moves (potentially every frame)
	// - We're using the LOOK TO function, which takes the position of the
	//    camera and the direction vector along which to look (as well as "up")
	// - Another option is the LOOK AT function, to look towards a specific
	//    point in 3D space

	for (unsigned int i = 0; i < MAX_CAMERAS; i++)
	{
		if (m_cameras[i])
		{
			XMFLOAT3 cameraPosition = m_cameras[i]->getPosition();

			XMFLOAT3 forwardFloat3 = m_cameras[i]->getForward();
			XMFLOAT3 upFloat3 = m_cameras[i]->getUp();

			XMVECTOR eye = XMLoadFloat3(&cameraPosition);
			XMVECTOR forward = XMLoadFloat3(&forwardFloat3);
			XMVECTOR up = XMLoadFloat3(&upFloat3);

			XMMATRIX viewMatrix = XMMatrixLookToLH(eye, forward, up);
			XMStoreFloat4x4(&m_viewMatrices[i], viewMatrix);
		}
	}
}

void CameraManager::updateProjectionMatrix(int width, int height)
{
	XMMATRIX perspective = XMMatrixPerspectiveFovLH(XM_PIDIV4, (float)width / height, 0.1f, 100.0f);
	XMStoreFloat4x4(&m_projectionMatrix, perspective);
}

const DirectX::XMMATRIX CameraManager::getViewMatrix(int index) const
{
	assert(index >= 0 && index < MAX_CAMERAS);
	return XMLoadFloat4x4(&m_viewMatrices[index]);
}

const DirectX::XMMATRIX CameraManager::getProjectionMatrix() const
{
	return XMLoadFloat4x4(&m_projectionMatrix);
}

Entity* CameraManager::getCameraEntity(int index) const
{
	assert(index >= 0 || index < 4);
	return m_cameras[index];
}

void CameraManager::assignCamera(int index, Entity* entity)
{
	assert(index >= 0 || index < 4);
	m_cameras[index] = entity;
}

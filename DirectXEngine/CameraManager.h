#pragma once

#include "Entity.h"

#define MAX_CAMERAS 4

class CameraManager
{
public:
	CameraManager();
	~CameraManager();

	void updateViewMatrices();
	void updateProjectionMatrix(int width, int height);

	const DirectX::XMMATRIX getViewMatrix(int index) const;
	const DirectX::XMMATRIX getProjectionMatrix() const;

	Entity* getCameraEntity(int index) const;

	void assignCamera(int cameraIndex, Entity* entity);

private:
	Entity* m_cameras[MAX_CAMERAS];

	DirectX::XMFLOAT4X4 m_viewMatrices[MAX_CAMERAS];
	DirectX::XMFLOAT4X4 m_projectionMatrix;
};
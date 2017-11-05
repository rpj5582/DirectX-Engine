#pragma once

#include "../Component/Transform.h"
#include "../Component/MeshRenderComponent.h"
#include "../Component/LightComponent.h"
#include "../Component/CameraComponent.h"

#include <DirectXMath.h>

class Renderer
{
public:
	Renderer(ID3D11DeviceContext* context);
	~Renderer();

	bool init();

	void render(const CameraComponent& mainCamera, DirectX::XMFLOAT4X4 projectionMatrix, Entity** entities, size_t entityCount, const GPU_LIGHT_DATA* lightData);

private:
	ID3D11DeviceContext* m_context;
};

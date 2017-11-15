#pragma once

#include "../Component/Transform.h"
#include "../Component/MeshRenderComponent.h"
#include "../Component/LightComponent.h"
#include "../Component/CameraComponent.h"

#include <DirectXMath.h>

#define SHADOW_MAP_SIZE 2048

class Renderer
{
public:
	Renderer(ID3D11Device* device, ID3D11DeviceContext* context);
	~Renderer();

	bool init();

	void prepareShadowMapPass();
	void renderShadowMapPass(Entity** entities, size_t entityCount, const LightComponent& light);

	void prepareMainPass(ID3D11RenderTargetView* backBufferRTV, ID3D11DepthStencilView* backBufferDSV);
	void renderMainPass(const CameraComponent& mainCamera, DirectX::XMFLOAT4X4 projectionMatrix, Entity** entities, size_t entityCount, const LightComponent* shadowMapLight, const GPU_LIGHT_DATA* lightData);

private:
	ID3D11Device* m_device;
	ID3D11DeviceContext* m_context;

	Texture* m_defaultShadowMap;
	ID3D11DepthStencilView* m_shadowMapDSV;
	ID3D11ShaderResourceView* m_shadowMapSRV;

	VertexShader* m_basicVertexShader;
	Sampler* m_shadowMapSampler;
};

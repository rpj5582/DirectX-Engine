#pragma once

#include "../Component/Transform.h"
#include "../Component/MeshRenderComponent.h"
#include "../Component/LightComponent.h"
#include "../Component/CameraComponent.h"

#include <DirectXMath.h>

#define MAX_SHADOWMAPS 16

class Renderer
{
public:
	Renderer(ID3D11Device* device, ID3D11DeviceContext* context);
	~Renderer();

	bool init();

	void prepareShadowMapPass(Texture* shadowMap);
	void renderShadowMapPass(Entity** entities, size_t entityCount, const LightComponent& light);

	void prepareMainPass(ID3D11RenderTargetView* backBufferRTV, ID3D11DepthStencilView* backBufferDSV);
	void renderMainPass(const CameraComponent& mainCamera, DirectX::XMFLOAT4X4 projectionMatrix, Entity*const * entities, size_t entityCount, const LightComponent*const * shadowLights, const GPU_LIGHT_DATA* lightData);

private:
	ID3D11Device* m_device;
	ID3D11DeviceContext* m_context;

	ID3D11RasterizerState* m_shadowMapRasterizerState;

	VertexShader* m_basicVertexShader;
	Sampler* m_shadowMapSampler;
};

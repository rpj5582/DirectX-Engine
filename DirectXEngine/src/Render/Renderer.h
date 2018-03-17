#pragma once
#include "IRenderer.h"

#include "../Component/Transform.h"
#include "../Component/MeshRenderComponent.h"
#include "../Component/LightComponent.h"
#include "../Component/CameraComponent.h"
#include "../Component/ICollider.h"

#include <DirectXMath.h>

#define MAX_LIGHTS 64
#define MAX_SHADOWMAPS 16

// The struct that should match the light data in the shaders.
struct GPU_LIGHT_DATA
{
	DirectX::XMFLOAT4 color;
	DirectX::XMFLOAT3 direction;
	float brightness;
	DirectX::XMFLOAT3 position;
	float specularity;
	float radius;
	float spotAngle;
	bool enabled;
	int type;
	bool shadowMapEnabled;
	int shadowType;
	DirectX::XMFLOAT2 padding;
};

struct GPU_SHADOW_MATRICES
{
	DirectX::XMFLOAT4X4 lightViewMatrix;
	DirectX::XMFLOAT4X4 lightProjectionMatrix;
};

class Renderer : public IRenderer
{
public:
	Renderer(ID3D11Device* device, ID3D11DeviceContext* context);
	~Renderer();

	bool init() override;

	void begin() override;
	void end() override;

	void prepareShadowMapPass(Texture* shadowMap);
	void renderShadowMapPass(Entity** entities, size_t entityCount, const LightComponent& light);

	void prepareMainPass(ID3D11RenderTargetView* backBufferRTV, ID3D11DepthStencilView* backBufferDSV, float width, float height);
	void renderMainPass(const CameraComponent& mainCamera, DirectX::XMFLOAT4X4 projectionMatrix, Entity*const * entities, size_t entityCount,
		const GPU_LIGHT_DATA* lightData, const GPU_SHADOW_MATRICES* shadowMatrices, ID3D11ShaderResourceView*const * shadowMapSRVs);

private:
	ID3D11Device* m_device;
	ID3D11DeviceContext* m_context;

	VertexShader* m_basicVertexShader;
	Sampler* m_shadowMapSampler;

	ID3D11RasterizerState* m_wireframeRasterizerState;
	ID3D11RasterizerState* m_shadowMapRasterizerState;

	ID3D11DepthStencilState* m_depthStencilStateDefault;
};

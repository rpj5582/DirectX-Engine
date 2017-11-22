#include "Renderer.h"

using namespace DirectX;

Renderer::Renderer(ID3D11Device* device, ID3D11DeviceContext* context)
{
	m_device = device;
	m_context = context;

	m_shadowMapRasterizerState = nullptr;

	m_basicVertexShader = nullptr;
	m_shadowMapSampler = nullptr;
}

Renderer::~Renderer()
{
	if (m_shadowMapRasterizerState) m_shadowMapRasterizerState->Release();

	m_device = nullptr;
	m_context = nullptr;

	m_basicVertexShader = nullptr;
	m_shadowMapSampler = nullptr;
}

bool Renderer::init()
{
	HRESULT hr = S_OK;

	D3D11_RASTERIZER_DESC shadowMapRasterizerDesc = {};
	shadowMapRasterizerDesc.FillMode = D3D11_FILL_SOLID;
	shadowMapRasterizerDesc.CullMode = D3D11_CULL_BACK;
	shadowMapRasterizerDesc.FrontCounterClockwise = FALSE;
	shadowMapRasterizerDesc.DepthClipEnable = TRUE;
	shadowMapRasterizerDesc.ScissorEnable = FALSE;
	shadowMapRasterizerDesc.MultisampleEnable = FALSE;
	shadowMapRasterizerDesc.AntialiasedLineEnable = FALSE;
	shadowMapRasterizerDesc.DepthBias = 1000;
	shadowMapRasterizerDesc.DepthBiasClamp = 0.0f;
	shadowMapRasterizerDesc.SlopeScaledDepthBias = 5.0f;

	hr = m_device->CreateRasterizerState(&shadowMapRasterizerDesc, &m_shadowMapRasterizerState);
	if (FAILED(hr))
	{
		Debug::error("Failed to create default rasterizer state.");
		return false;
	}

	m_basicVertexShader = AssetManager::getAsset<VertexShader>(BASIC_SHADER_VERTEX);
	if (!m_basicVertexShader)
	{
		Debug::error("Renderer failed to get basic vertex shader.");
		return false;
	}

	m_shadowMapSampler = AssetManager::getAsset<Sampler>(SHADOWMAP_SAMPLER);
	if (!m_shadowMapSampler)
	{
		Debug::error("Renderer failed to get shadow map sampler.");
		return false;
	}

	return true;
}

void Renderer::prepareShadowMapPass(Texture* shadowMap)
{
	if (!shadowMap)
	{
		Debug::warning("Could not prepare shadow pass since no shadow map was given.");
		return;
	}

	ID3D11DepthStencilView* shadowMapDSV = shadowMap->getDSV();

	m_context->ClearDepthStencilView(shadowMapDSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	m_context->RSSetState(m_shadowMapRasterizerState);

	ID3D11RenderTargetView* nullRTV = nullptr;
	m_context->OMSetRenderTargets(1, &nullRTV, shadowMapDSV);

	D3D11_VIEWPORT viewport = {};
	viewport.TopLeftX = viewport.TopLeftY = 0;
	viewport.MinDepth = 0;
	viewport.MaxDepth = 1;
	viewport.Width = (float)shadowMap->getWidth();
	viewport.Height = (float)shadowMap->getHeight();
	m_context->RSSetViewports(1, &viewport);

	m_basicVertexShader->SetShader();
	m_context->PSSetShader(nullptr, nullptr, 0);
}

void Renderer::renderShadowMapPass(Entity** entities, size_t entityCount, const LightComponent& light)
{
	XMFLOAT4X4 view = light.getViewMatrix();
	XMMATRIX viewMatrixT = XMMatrixTranspose(XMLoadFloat4x4(&view));
	XMFLOAT4X4 viewT;
	XMStoreFloat4x4(&viewT, viewMatrixT);

	XMMATRIX projectionMatrixT = XMMatrixTranspose(XMLoadFloat4x4(&light.getProjectionMatrix()));
	XMFLOAT4X4 projT;
	XMStoreFloat4x4(&projT, projectionMatrixT);

	unsigned int stride = sizeof(Vertex);
	unsigned int offset = 0;

	for (unsigned int i = 0; i < entityCount; i++)
	{
		//  Don't use disabled entities
		if (!entities[i]->getEnabled()) continue;

		Transform* transform = entities[i]->getComponent<Transform>();
		MeshRenderComponent* meshRenderComponent = entities[i]->getComponent<MeshRenderComponent>();
		if (meshRenderComponent && meshRenderComponent->enabled && meshRenderComponent->castShadows && transform)
		{
			XMFLOAT4X4 world = transform->getWorldMatrix();
			XMMATRIX worldMatrixT = XMMatrixTranspose(XMLoadFloat4x4(&world));
			XMFLOAT4X4 worldT;
			XMStoreFloat4x4(&worldT, worldMatrixT);

			m_basicVertexShader->SetMatrix4x4("world", worldT);
			m_basicVertexShader->SetMatrix4x4("view", viewT);
			m_basicVertexShader->SetMatrix4x4("projection", projT);
			m_basicVertexShader->CopyBufferData("matrices");

			Mesh* mesh = meshRenderComponent->getMesh();
			if (mesh)
			{
				ID3D11Buffer* vertexBuffer = mesh->getVertexBuffer();
				ID3D11Buffer* indexBuffer = mesh->getIndexBuffer();

				m_context->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
				m_context->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);

				m_context->DrawIndexed((UINT)mesh->getIndexCount(), 0, 0);								
			}
		}
	}
}

void Renderer::prepareMainPass(ID3D11RenderTargetView* backBufferRTV, ID3D11DepthStencilView* backBufferDSV)
{
	m_context->OMSetRenderTargets(1, &backBufferRTV, backBufferDSV);

	m_context->ClearRenderTargetView(backBufferRTV, Colors::CornflowerBlue);
	m_context->ClearDepthStencilView(backBufferDSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	m_context->RSSetState(nullptr);

	D3D11_VIEWPORT viewport = {};
	viewport.TopLeftX = viewport.TopLeftY = 0;
	viewport.MinDepth = 0;
	viewport.MaxDepth = 1;
	viewport.Width = (float)Window::getWidth();
	viewport.Height = (float)Window::getHeight();
	m_context->RSSetViewports(1, &viewport);
}

void Renderer::renderMainPass(const CameraComponent& mainCamera, DirectX::XMFLOAT4X4 projectionMatrix, Entity*const * entities, size_t entityCount,
	const GPU_LIGHT_DATA* lightData, const GPU_SHADOW_MATRICES* shadowMatrices, ID3D11ShaderResourceView*const * shadowMapSRVs)
{
	Transform* mainCameraTransform = mainCamera.getEntity().getComponent<Transform>();
	if (!mainCameraTransform) return;

	XMFLOAT4X4 view = mainCamera.getViewMatrix();
	XMMATRIX viewMatrixT = XMMatrixTranspose(XMLoadFloat4x4(&view));
	XMFLOAT4X4 viewT;
	XMStoreFloat4x4(&viewT, viewMatrixT);

	XMMATRIX projectionMatrixT = XMMatrixTranspose(XMLoadFloat4x4(&projectionMatrix));
	XMFLOAT4X4 projT;
	XMStoreFloat4x4(&projT, projectionMatrixT);

	unsigned int stride = sizeof(Vertex);
	unsigned int offset = 0;

	for (unsigned int i = 0; i < entityCount; i++)
	{
		//  Don't use disabled entities
		if (!entities[i]->getEnabled()) continue;

		// Don't draw the entity if it can't be seen anyway
		Transform* transform = entities[i]->getComponent<Transform>();
		MeshRenderComponent* meshRenderComponent = entities[i]->getComponent<MeshRenderComponent>();
		if (meshRenderComponent && meshRenderComponent->enabled && transform)
		{
			Material* material = meshRenderComponent->getMaterial();
			Mesh* mesh = meshRenderComponent->getMesh();
			if (material && mesh)
			{
				material->useMaterial();

				SimpleVertexShader* vertexShader = material->getVertexShader();
				SimplePixelShader* pixelShader = material->getPixelShader();

				XMFLOAT4X4 world = transform->getWorldMatrix();
				XMMATRIX worldMatrixT = XMMatrixTranspose(XMLoadFloat4x4(&world));
				XMFLOAT4X4 worldT;
				XMStoreFloat4x4(&worldT, worldMatrixT);

				XMFLOAT4X4 worldMatrixInverse = transform->getInverseWorldMatrix();

				vertexShader->SetMatrix4x4("world", worldT);
				vertexShader->SetMatrix4x4("view", viewT);
				vertexShader->SetMatrix4x4("projection", projT);
				vertexShader->SetMatrix4x4("worldInverseTranspose", worldMatrixInverse);
				vertexShader->SetData("shadowMatrices", &shadowMatrices[0], sizeof(GPU_SHADOW_MATRICES) * MAX_SHADOWMAPS);

				vertexShader->CopyBufferData("matrices");

				pixelShader->SetSamplerState("shadowMapSampler", m_shadowMapSampler->getSamplerState());

				pixelShader->SetFloat3("cameraWorldPosition", mainCameraTransform->getPosition());
				pixelShader->CopyBufferData("camera");

				if (!Debug::inPlayMode && entities[i]->selected)
				{
					pixelShader->SetInt("renderStyle", (int)SOLID_WIREFRAME);
					pixelShader->SetFloat4("wireColor", XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f));
				}
				else
				{
					pixelShader->SetInt("renderStyle", (int)meshRenderComponent->getRenderStyle());
					pixelShader->SetFloat4("wireColor", meshRenderComponent->getWireframeColor());
				}

				pixelShader->CopyBufferData("renderStyle");

				pixelShader->SetShaderResourceViewArray("shadowMaps", shadowMapSRVs, MAX_SHADOWMAPS);

				pixelShader->SetData("lights", lightData, sizeof(GPU_LIGHT_DATA) * MAX_LIGHTS);
				pixelShader->CopyBufferData("lighting");

				ID3D11Buffer* vertexBuffer = mesh->getVertexBuffer();
				ID3D11Buffer* indexBuffer = mesh->getIndexBuffer();

				m_context->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
				m_context->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);

				// Finally do the actual drawing
				//  - Do this ONCE PER OBJECT you intend to draw
				//  - This will use all of the currently set DirectX "stuff" (shaders, buffers, etc)
				//  - DrawIndexed() uses the currently set INDEX BUFFER to look up corresponding
				//     vertices in the currently set VERTEX BUFFER
				m_context->DrawIndexed(
					(UINT)mesh->getIndexCount(),			// The number of indices to use (we could draw a subset if we wanted)
					0,								// Offset to the first index we want to use
					0);								// Offset to add to each index when looking up vertices

				ID3D11ShaderResourceView* empty[MAX_SHADOWMAPS];
				for (unsigned int i = 0; i < MAX_SHADOWMAPS; i++)
				{
					empty[i] = nullptr;
				}
				pixelShader->SetShaderResourceViewArray("shadowMaps", &empty[0], MAX_SHADOWMAPS);
			}
		}
	}
}

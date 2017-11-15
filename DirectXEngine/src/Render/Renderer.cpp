#include "Renderer.h"

using namespace DirectX;

Renderer::Renderer(ID3D11Device* device, ID3D11DeviceContext* context, ID3D11DepthStencilView* shadowMapDSV, ID3D11ShaderResourceView* shadowMapSRV)
{
	m_device = device;
	m_context = context;

	m_defaultShadowMap = nullptr;
	m_shadowMapDSV = shadowMapDSV;
	m_shadowMapSRV = shadowMapSRV;

	m_basicVertexShader = nullptr;
	m_shadowMapSampler = nullptr;
}

Renderer::~Renderer()
{
	m_defaultShadowMap = nullptr;
	if (m_shadowMapDSV) m_shadowMapDSV->Release();
	if (m_shadowMapSRV) m_shadowMapSRV->Release();

	m_device = nullptr;
	m_context = nullptr;

	m_basicVertexShader = nullptr;
	m_shadowMapSampler = nullptr;
}

bool Renderer::init()
{
	m_defaultShadowMap = AssetManager::getAsset<Texture>(DEFAULT_TEXTURE_WHITE);

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

void Renderer::prepareShadowMapPass()
{
	m_context->ClearDepthStencilView(m_shadowMapDSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	ID3D11RenderTargetView* nullRTV = nullptr;
	m_context->OMSetRenderTargets(1, &nullRTV, m_shadowMapDSV);

	D3D11_VIEWPORT viewport = {};
	viewport.TopLeftX = viewport.TopLeftY = 0;
	viewport.MinDepth = 0;
	viewport.MaxDepth = 1;
	viewport.Width = SHADOW_MAP_SIZE;
	viewport.Height = SHADOW_MAP_SIZE;
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
		if (meshRenderComponent && meshRenderComponent->enabled && transform)
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

	D3D11_VIEWPORT viewport = {};
	viewport.TopLeftX = viewport.TopLeftY = 0;
	viewport.MinDepth = 0;
	viewport.MaxDepth = 1;
	viewport.Width = (float)Window::getWidth();
	viewport.Height = (float)Window::getHeight();
	m_context->RSSetViewports(1, &viewport);
}

void Renderer::renderMainPass(const CameraComponent& mainCamera, XMFLOAT4X4 projectionMatrix, Entity** entities, size_t entityCount, const LightComponent* shadowMapLight, const GPU_LIGHT_DATA* lightData)
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

	XMFLOAT4X4 lightViewT;
	XMFLOAT4X4 lightProjT;

	if (shadowMapLight)
	{
		XMFLOAT4X4 lightView = shadowMapLight->getViewMatrix();
		XMMATRIX lightViewMatrixT = XMMatrixTranspose(XMLoadFloat4x4(&lightView));
		XMStoreFloat4x4(&lightViewT, lightViewMatrixT);

		XMFLOAT4X4 lightProj = shadowMapLight->getProjectionMatrix();
		XMMATRIX lightProjectionMatrixT = XMMatrixTranspose(XMLoadFloat4x4(&lightProj));
		XMStoreFloat4x4(&lightProjT, lightProjectionMatrixT);
	}
	else
	{
		// If there isn't a light casting shadows, use the camera in order to have valid view and projection matrices
		XMFLOAT4X4 lightView = mainCamera.getViewMatrix();
		XMMATRIX lightViewMatrixT = XMMatrixTranspose(XMLoadFloat4x4(&lightView));
		XMStoreFloat4x4(&lightViewT, lightViewMatrixT);

		XMFLOAT4X4 lightProj = Window::getProjectionMatrix();
		XMMATRIX lightProjectionMatrixT = XMMatrixTranspose(XMLoadFloat4x4(&lightProj));
		XMStoreFloat4x4(&lightProjT, lightProjectionMatrixT);
	}

	unsigned int stride = sizeof(Vertex);
	unsigned int offset = 0;

	for (unsigned int i = 0; i < entityCount; i++)
	{
		//  Don't use disabled entities
		if (!entities[i]->getEnabled()) continue;

		Transform* transform = entities[i]->getComponent<Transform>();
		RenderComponent* renderComponent = entities[i]->getComponent<RenderComponent>();
		if (renderComponent && renderComponent->enabled && transform)
		{
			Material* material = renderComponent->getMaterial();
			if (material)
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
				vertexShader->SetMatrix4x4("lightView", lightViewT);
				vertexShader->SetMatrix4x4("lightProjection", lightProjT);
				vertexShader->CopyBufferData("matrices");

				// Don't draw the entity if it can't be seen anyway
				MeshRenderComponent* meshRenderComponent = entities[i]->getComponent<MeshRenderComponent>();
				if (meshRenderComponent)
				{
					pixelShader->SetSamplerState("shadowMapSampler", m_shadowMapSampler->getSampler());

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

					if (meshRenderComponent->receiveShadows)
						pixelShader->SetShaderResourceView("shadowMap", m_shadowMapSRV);
					else
						pixelShader->SetShaderResourceView("shadowMap", m_defaultShadowMap->getSRV());

					Mesh* mesh = meshRenderComponent->getMesh();
					if (mesh)
					{
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
					}
				}

				pixelShader->SetShaderResourceView("shadowMap", nullptr);
			}
		}
	}
}

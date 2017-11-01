#include "Renderer.h"

using namespace DirectX;

Renderer::Renderer(ID3D11DeviceContext* context)
{
	m_context = context;
}

Renderer::~Renderer()
{
}

bool Renderer::init()
{
	return true;
}

void Renderer::render(const CameraComponent& mainCamera, const XMMATRIX& projectionMatrix, Entity** entities, unsigned int entityCount, const GPU_LIGHT_DATA* lightData)
{
	Transform* mainCameraTransform = mainCamera.getEntity().getComponent<Transform>();
	if (!mainCameraTransform) return;

	XMMATRIX viewMatrixT = XMMatrixTranspose(mainCamera.getViewMatrix());
	XMFLOAT4X4 viewMatrix4x4;
	XMStoreFloat4x4(&viewMatrix4x4, viewMatrixT);

	XMMATRIX projectionMatrixT = XMMatrixTranspose(projectionMatrix);
	XMFLOAT4X4 projectionMatrix4x4;
	XMStoreFloat4x4(&projectionMatrix4x4, projectionMatrixT);

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

				XMMATRIX worldMatrixT = XMMatrixTranspose(transform->getWorldMatrix());
				XMFLOAT4X4 worldMatrix4x4;
				XMStoreFloat4x4(&worldMatrix4x4, worldMatrixT);

				XMMATRIX worldMatrixIT = transform->getInverseWorldMatrix();
				XMFLOAT4X4 worldMatrixInverseTranspose4x4;
				XMStoreFloat4x4(&worldMatrixInverseTranspose4x4, worldMatrixIT);

				vertexShader->SetMatrix4x4("world", worldMatrix4x4);
				vertexShader->SetMatrix4x4("view", viewMatrix4x4);
				vertexShader->SetMatrix4x4("projection", projectionMatrix4x4);
				vertexShader->SetMatrix4x4("worldInverseTranspose", worldMatrixInverseTranspose4x4);
				vertexShader->CopyBufferData("matrices");

				// Don't draw the entity if it can't be seen anyway
				MeshRenderComponent* meshRenderComponent = entities[i]->getComponent<MeshRenderComponent>();
				if (meshRenderComponent)
				{
					pixelShader->SetFloat3("cameraWorldPosition", mainCameraTransform->getPosition());
					pixelShader->CopyBufferData("camera");

					pixelShader->SetInt("renderStyle", (int)meshRenderComponent->getRenderStyle());
					pixelShader->CopyBufferData("renderStyle");

					Mesh* mesh = meshRenderComponent->getMesh();
					if (mesh)
					{
						unsigned int stride = sizeof(Vertex);
						unsigned int offset = 0;

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
							mesh->getIndexCount(),			// The number of indices to use (we could draw a subset if we wanted)
							0,								// Offset to the first index we want to use
							0);								// Offset to add to each index when looking up vertices
					}
				}
			}
		}
	}
}

#include "Renderer.h"

#include "Scene.h"

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

void Renderer::render(Scene* scene, GPU_LIGHT_DATA* lightData, unsigned int lightCount)
{
	// Only use the main camera when rendering to the screen.
	Camera* mainCamera = scene->getMainCamera();
	Transform* mainCameraTransform = scene->getComponentOfEntity<Transform>(mainCamera->getEntity());
	if (!mainCameraTransform) return;

	XMMATRIX viewMatrix = XMMatrixTranspose(mainCamera->getViewMatrix());
	XMFLOAT4X4 viewMatrix4x4;
	XMStoreFloat4x4(&viewMatrix4x4, viewMatrix);

	XMMATRIX projectionMatrix = XMMatrixTranspose(scene->getProjectionMatrix());
	XMFLOAT4X4 projectionMatrix4x4;
	XMStoreFloat4x4(&projectionMatrix4x4, projectionMatrix);

	unsigned int* entities;
	unsigned int entityCount;
	scene->getAllEntities(&entities, &entityCount);

	unsigned int stride = sizeof(Vertex);
	unsigned int offset = 0;
	for (unsigned int i = 0; i < entityCount; i++)
	{
		Transform* transform = scene->getComponentOfEntity<Transform>(entities[i]);
		RenderComponent* renderComponent = scene->getComponentOfEntity<RenderComponent>(entities[i]);
		if (renderComponent && transform)
		{
			Material* material = renderComponent->getMaterial();
			material->useMaterial();
			SimpleVertexShader* vertexShader = material->getVertexShader();
			SimplePixelShader* pixelShader = material->getPixelShader();

			XMMATRIX worldMatrix = XMMatrixTranspose(transform->getWorldMatrix());
			XMFLOAT4X4 worldMatrix4x4;
			XMStoreFloat4x4(&worldMatrix4x4, worldMatrix);

			XMMATRIX worldMatrixInverseTranspose = XMMatrixInverse(nullptr, worldMatrix);
			XMFLOAT4X4 worldMatrixInverseTranspose4x4;
			XMStoreFloat4x4(&worldMatrixInverseTranspose4x4, worldMatrixInverseTranspose);

			vertexShader->SetMatrix4x4("world", worldMatrix4x4);
			vertexShader->SetMatrix4x4("view", viewMatrix4x4);
			vertexShader->SetMatrix4x4("projection", projectionMatrix4x4);
			vertexShader->SetMatrix4x4("worldInverseTranspose", worldMatrixInverseTranspose4x4);
			vertexShader->CopyBufferData("matrices");

			// Don't draw the entity if it can't be seen anyway
			MeshRenderComponent* meshRenderComponent = scene->getComponentOfEntity<MeshRenderComponent>(entities[i]);
			if (meshRenderComponent)
			{
				pixelShader->SetFloat3("cameraWorldPosition", mainCameraTransform->getPosition());
				pixelShader->CopyBufferData("camera");

				Mesh* mesh = meshRenderComponent->getMesh();
				if (mesh)
				{
					RenderStyle renderStyle = renderComponent->getRenderStyle();
					switch (renderStyle)
					{
					case RenderStyle::SOLID:
						scene->drawInWireframeMode(false);
						renderMesh(pixelShader, mesh, lightData, lightCount);
						break;

					case RenderStyle::WIREFRAME:
						scene->drawInWireframeMode(true);
						renderMeshWithoutLighting(pixelShader, mesh);
						break;

					case RenderStyle::SOLID_WIREFRAME:
						// Only render in solid wireframe if in debug mode, since it requires rendering the mesh twice.
					#if defined(DEBUG) || defined(_DEBUG)
					 	scene->drawInWireframeMode(false);
						renderMesh(pixelShader, mesh, lightData, lightCount);
					#endif
						scene->drawInWireframeMode(true);
						renderMeshWithoutLighting(pixelShader, mesh);
						break;

					default:
						break;
					}
				}
			}
		}
	}
}

void Renderer::renderMesh(SimplePixelShader* pixelShader, Mesh* mesh, GPU_LIGHT_DATA* lightData, unsigned int lightCount)
{
	unsigned int stride = sizeof(Vertex);
	unsigned int offset = 0;

	if (!lightData)
	{
		renderMeshWithoutLighting(pixelShader, mesh);
		return;
	}

	pixelShader->SetInt("lightCount", lightCount);
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

void Renderer::renderMeshWithoutLighting(SimplePixelShader* pixelShader, Mesh* mesh)
{

	unsigned int stride = sizeof(Vertex);
	unsigned int offset = 0;

	pixelShader->SetInt("lightCount", 0);
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

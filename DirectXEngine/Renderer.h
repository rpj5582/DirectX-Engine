#pragma once

#include "Transform.h"
#include "MeshRenderComponent.h"

#include <DirectXMath.h>

class Scene;
struct GPU_LIGHT_DATA;
enum RenderStyle;

class Renderer
{
public:
	Renderer();
	~Renderer();

	// Renders the scene.
	void render(Scene* scene, ID3D11DeviceContext* context, GPU_LIGHT_DATA* lightData, unsigned int lightCount);

private:
	void renderMeshAndLighting(ID3D11DeviceContext* context, SimplePixelShader* pixelShader, Mesh* mesh, GPU_LIGHT_DATA* lightData, unsigned int lightCount);
	void renderMeshWithoutLighting(ID3D11DeviceContext* context, SimplePixelShader* pixelShader, Mesh* mesh);
};
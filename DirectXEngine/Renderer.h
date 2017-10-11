#pragma once

#include "Transform.h"
#include "MeshRenderComponent.h"
#include "LightComponent.h"

#include <DirectXMath.h>

class Scene;
struct GPU_LIGHT_DATA;
enum RenderStyle;

class Renderer
{
public:
	Renderer(ID3D11DeviceContext* context);
	~Renderer();

	bool init();

	void render(Scene* scene, const GPU_LIGHT_DATA* lightData, unsigned int lightCount);

private:
	void renderMesh(SimplePixelShader* pixelShader, const Mesh* mesh, const GPU_LIGHT_DATA* lightData, unsigned int lightCount);
	void renderMeshWithoutLighting(SimplePixelShader* pixelShader, const Mesh* mesh);

	ID3D11DeviceContext* m_context;
};

#pragma once

#include "Transform.h"
#include "MeshRenderComponent.h"

#include <DirectXMath.h>

class Scene;
struct GPU_LIGHT_DATA;

class Renderer
{
public:
	Renderer();
	~Renderer();

	virtual void render(Scene* scene, ID3D11DeviceContext* context, GPU_LIGHT_DATA* lightData, unsigned int lightCount);

private:

};
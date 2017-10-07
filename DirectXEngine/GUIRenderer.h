#pragma once

#include "GUIComponent.h"
#include "GUITransform.h"

#include <SpriteBatch.h>

class Scene;

class GUIRenderer
{
public:
	GUIRenderer(ID3D11DeviceContext* context);
	~GUIRenderer();

	bool init();

	void render(Scene* scene, ID3D11BlendState* blendState, ID3D11DepthStencilState* depthStencilState, GUIComponent** guiComponents, unsigned int guiCount);

private:
	ID3D11DeviceContext* m_context;
	DirectX::SpriteBatch* m_spriteBatch;
};
#pragma once

#include "GUITransform.h"
#include "GUISpriteComponent.h"
#include "GUITextComponent.h"
#include "GUIButtonComponent.h"

#include <SpriteBatch.h>

class Scene;

class GUIRenderer
{
public:
	GUIRenderer(ID3D11Device* device, ID3D11DeviceContext* context);
	~GUIRenderer();

	bool init();

	void begin(ID3D11BlendState* blendState, ID3D11DepthStencilState* depthStencilState);
	void render(Scene* scene, const GUIComponent* const* guiComponents, unsigned int guiCount);
	void end();

private:
	ID3D11Device* m_device;
	ID3D11DeviceContext* m_context;
	DirectX::SpriteBatch* m_spriteBatch;
};
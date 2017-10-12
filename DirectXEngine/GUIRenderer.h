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
	GUIRenderer(ID3D11DeviceContext* context);
	~GUIRenderer();

	void begin(ID3D11BlendState* blendState, ID3D11DepthStencilState* depthStencilState);
	void render(const GUIComponent* const* guiComponents, unsigned int guiCount);
	void end();

private:
	ID3D11DeviceContext* m_context;
	DirectX::SpriteBatch* m_spriteBatch;
};

#pragma once

#include "../Component/GUITransform.h"
#include "../Component/GUISpriteComponent.h"
#include "../Component/GUITextComponent.h"
#include "../Component/GUIButtonComponent.h"

#include <SpriteBatch.h>

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

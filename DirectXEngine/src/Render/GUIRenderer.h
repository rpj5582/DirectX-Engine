#pragma once
#include "IRenderer.h"

#include "../Component/GUIComponent.h"

#include <SpriteBatch.h>

class GUIRenderer : public IRenderer
{
public:
	GUIRenderer(ID3D11Device* device, ID3D11DeviceContext* context);
	~GUIRenderer();

	bool init() override;

	void begin() override;
	void render(const GUIComponent* const* guiComponents, size_t guiCount);
	void end() override;

private:
	ID3D11Device* m_device;
	ID3D11DeviceContext* m_context;
	DirectX::SpriteBatch* m_spriteBatch;

	ID3D11BlendState* m_blendState;
	ID3D11DepthStencilState* m_depthStencilStateRead;
};

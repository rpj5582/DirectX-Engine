#include "GUIRenderer.h"

using namespace DirectX;

GUIRenderer::GUIRenderer(ID3D11DeviceContext* context)
{
	m_context = context;
	m_spriteBatch = new SpriteBatch(m_context);
}

GUIRenderer::~GUIRenderer()
{
	delete m_spriteBatch;
}

void GUIRenderer::begin(ID3D11BlendState* blendState, ID3D11DepthStencilState* depthStencilState)
{
	m_spriteBatch->Begin(SpriteSortMode_Deferred, blendState, nullptr, depthStencilState);
}

void GUIRenderer::render(const GUIComponent* const* guiComponents, unsigned int guiCount)
{
	for (unsigned int i = 0; i < guiCount; i++)
	{
		guiComponents[i]->draw(*m_spriteBatch);
	}
}

void GUIRenderer::end()
{
	m_spriteBatch->End();
}

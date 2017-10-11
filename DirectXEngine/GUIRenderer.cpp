#include "GUIRenderer.h"

using namespace DirectX;

GUIRenderer::GUIRenderer(ID3D11Device* device, ID3D11DeviceContext* context)
{
	m_device = device;
	m_context = context;
	m_spriteBatch = nullptr;
}

GUIRenderer::~GUIRenderer()
{
	delete m_spriteBatch;
}

bool GUIRenderer::init()
{
	m_spriteBatch = new SpriteBatch(m_context);
	return true;
}

void GUIRenderer::begin(ID3D11BlendState* blendState, ID3D11DepthStencilState* depthStencilState)
{
	m_spriteBatch->Begin(SpriteSortMode_Deferred, blendState, nullptr, depthStencilState);
}

void GUIRenderer::render(Scene* scene, const GUIComponent* const* guiComponents, unsigned int guiCount)
{
	for (unsigned int i = 0; i < guiCount; i++)
	{
		if(guiComponents[i]->enabled)
			guiComponents[i]->draw(scene, m_spriteBatch);
	}
}

void GUIRenderer::end()
{
	m_spriteBatch->End();
}

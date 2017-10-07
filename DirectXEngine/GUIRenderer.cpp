#include "GUIRenderer.h"

#include "Scene.h"

using namespace DirectX;

GUIRenderer::GUIRenderer(ID3D11DeviceContext* context)
{
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

void GUIRenderer::render(Scene* scene, ID3D11BlendState* blendState, ID3D11DepthStencilState* depthStencilState, GUIComponent** guiComponents, unsigned int guiCount)
{
	m_spriteBatch->Begin(SpriteSortMode_Deferred, blendState, nullptr, depthStencilState);

	for (unsigned int i = 0; i < guiCount; i++)
	{
		ID3D11ShaderResourceView* textureSRV = guiComponents[i]->getTextureSRV();
		if (!textureSRV) continue;

		GUITransform* guiTransform = scene->getComponentOfEntity<GUITransform>(guiComponents[i]->getEntity());
		if (!guiTransform) continue;

		XMFLOAT2 position = guiTransform->getPosition();
		float rotation = guiTransform->getRotation();
		XMFLOAT2 size = guiTransform->getSize();
		m_spriteBatch->Draw(textureSRV, position, nullptr, Colors::White, sinf(XMConvertToRadians(rotation)), XMFLOAT2(0.5f, 0.5f), size);
	}

	m_spriteBatch->End();
}

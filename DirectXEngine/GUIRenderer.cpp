#include "GUIRenderer.h"

#include "Scene.h"

using namespace DirectX;

GUIRenderer::GUIRenderer(ID3D11Device* device, ID3D11DeviceContext* context)
{
	m_device = device;
	m_context = context;
	m_spriteBatch = nullptr;
	m_spriteFont = nullptr;
}

GUIRenderer::~GUIRenderer()
{
	delete m_spriteBatch;
	delete m_spriteFont;
}

bool GUIRenderer::init()
{
	m_spriteBatch = new SpriteBatch(m_context);
	m_spriteFont = new SpriteFont(m_device, L"Assets/Fonts/Arial.spritefont");
	return true;
}

void GUIRenderer::render(Scene* scene, ID3D11BlendState* blendState, ID3D11DepthStencilState* depthStencilState, GUIComponent** guiComponents, unsigned int guiCount)
{
	m_spriteBatch->Begin(SpriteSortMode_Deferred, blendState, nullptr, depthStencilState);

	m_spriteFont->DrawString(m_spriteBatch, L"TEST", XMFLOAT2());
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

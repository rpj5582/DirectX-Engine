#include "GUIRenderer.h"

#include "Scene.h"

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

void GUIRenderer::renderSprites(Scene* scene, GUISpriteComponent** guiSpriteComponents, unsigned int guiSpriteCount)
{
	for (unsigned int i = 0; i < guiSpriteCount; i++)
	{
		GUITransform* guiTransform = scene->getComponentOfEntity<GUITransform>(guiSpriteComponents[i]->getEntity());
		if (!guiTransform) continue;

		ID3D11ShaderResourceView* textureSRV = guiSpriteComponents[i]->getTextureSRV();
		if (!textureSRV) continue;

		XMFLOAT2 position = guiTransform->getPosition();
		float rotation = guiTransform->getRotation();
		XMFLOAT2 size = guiTransform->getSize();
		XMFLOAT2 origin = guiTransform->getOrigin();
		XMFLOAT4 colorFloat = guiSpriteComponents[i]->getColor();
		XMVECTORF32 color = { colorFloat.x, colorFloat.y, colorFloat.z, colorFloat.w };
		m_spriteBatch->Draw(textureSRV, position, nullptr, color, sinf(XMConvertToRadians(rotation)), origin, size);
	}
}

void GUIRenderer::renderText(Scene* scene, GUITextComponent** guiTextComponents, unsigned int guiTextCount)
{
	for (unsigned int i = 0; i < guiTextCount; i++)
	{
		GUITransform* guiTransform = scene->getComponentOfEntity<GUITransform>(guiTextComponents[i]->getEntity());
		if (!guiTransform) continue;

		SpriteFont* font = guiTextComponents[i]->getFont();
		if (!font) continue;

		std::string text = guiTextComponents[i]->getText();
		if (!text.empty())
		{
			XMFLOAT2 position = guiTransform->getPosition();
			float rotation = guiTransform->getRotation();
			XMFLOAT2 origin = guiTransform->getOrigin();
			XMFLOAT4 colorFloat = guiTextComponents[i]->getColor();
			XMVECTORF32 color = { colorFloat.x, colorFloat.y, colorFloat.z, colorFloat.w };

			font->DrawString(m_spriteBatch, std::wstring(text.begin(), text.end()).c_str(), position, color, sinf(XMConvertToRadians(rotation)), origin);
		}
	}
}

void GUIRenderer::end()
{
	m_spriteBatch->End();
}

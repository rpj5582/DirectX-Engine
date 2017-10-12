#include "GUIButtonComponent.h"

#include "GUITransform.h"

using namespace DirectX;

GUIButtonComponent::GUIButtonComponent(Entity& entity) : GUISpriteComponent(entity)
{
	m_onClick = nullptr;
	m_isClicking = false;

	m_font = nullptr;
	m_text = "";
	m_textColor = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
}

GUIButtonComponent::~GUIButtonComponent()
{
	entity.unsubscribeMouseDown(this);
	entity.unsubscribeMouseUp(this);
}

void GUIButtonComponent::init()
{
	GUISpriteComponent::init();

	entity.subscribeMouseDown(this);
	entity.subscribeMouseUp(this);

	m_font = AssetManager::getFont("Arial_16pt");
}

void GUIButtonComponent::draw(DirectX::SpriteBatch& spriteBatch) const
{
	GUITransform* guiTransform = entity.getComponent<GUITransform>();
	if (!guiTransform) return;

	if (!m_textureSRV) return;

	XMFLOAT2 position = guiTransform->getPosition();
	float rotation = guiTransform->getRotation();
	XMFLOAT2 origin = guiTransform->getOrigin();
	XMFLOAT2 size = guiTransform->getSize();
	XMVECTORF32 color = { m_color.x, m_color.y, m_color.z, m_color.w };
	XMVECTORF32 textColor = { m_textColor.x, m_textColor.y, m_textColor.z, m_textColor.w };
	spriteBatch.Draw(m_textureSRV, position, nullptr, color, sinf(XMConvertToRadians(rotation)), origin, size);

	m_font->DrawString(&spriteBatch, std::wstring(m_text.begin(), m_text.end()).c_str(), position, textColor, sinf(XMConvertToRadians(rotation)), origin);
}

void GUIButtonComponent::loadFromJSON(rapidjson::Value& dataObject)
{
	GUISpriteComponent::loadFromJSON(dataObject);


}

DirectX::SpriteFont* GUIButtonComponent::getFont() const
{
	return m_font;
}

void GUIButtonComponent::setFont(DirectX::SpriteFont* font)
{
	m_font = font;
}

std::string GUIButtonComponent::getText() const
{
	return m_text;
}

void GUIButtonComponent::setText(std::string text)
{
	m_text = text;
}

DirectX::XMFLOAT4 GUIButtonComponent::getTextColor() const
{
	return m_textColor;
}

void GUIButtonComponent::setTextColor(DirectX::XMFLOAT4 color)
{
	m_textColor = color;
}

void GUIButtonComponent::onMouseDown(WPARAM buttonState, int x, int y)
{
	GUITransform* guiTransform = entity.getComponent<GUITransform>();
	if (!guiTransform) return;

	if (guiTransform->containsPoint(x, y))
		m_isClicking = true;
}

void GUIButtonComponent::onMouseUp(WPARAM buttonState, int x, int y)
{
	if (m_isClicking)
	{
		GUITransform* guiTransform = entity.getComponent<GUITransform>();
		if (!guiTransform) return;
		if (!m_onClick) return;

		if (guiTransform->containsPoint(x, y))
			m_onClick();
	}

	m_isClicking = false;
}

#include "GUITextComponent.h"

#include "GUITransform.h"

using namespace DirectX;

GUITextComponent::GUITextComponent(Entity& entity) : GUIComponent(entity)
{
	m_font = nullptr;
	m_text = "";
}

GUITextComponent::~GUITextComponent()
{
}

void GUITextComponent::init()
{
	GUIComponent::init();

	m_font = AssetManager::getFont("Arial_16pt");
}

void GUITextComponent::loadFromJSON(rapidjson::Value& dataObject)
{
	GUIComponent::loadFromJSON(dataObject);

	rapidjson::Value::MemberIterator font = dataObject.FindMember("font");
	if (font != dataObject.MemberEnd())
	{
		m_font = AssetManager::getFont(font->value.GetString());
	}

	rapidjson::Value::MemberIterator text = dataObject.FindMember("text");
	if (text != dataObject.MemberEnd())
	{
		m_text = text->value.GetString();
	}
}

void GUITextComponent::draw(DirectX::SpriteBatch& spriteBatch) const
{
	GUITransform* guiTransform = entity.getComponent<GUITransform>();
	if (!guiTransform) return;

	if (!m_font) return;

	if (!m_text.empty())
	{
		XMFLOAT2 position = guiTransform->getPosition();
		float rotation = guiTransform->getRotation();
		XMFLOAT2 origin = guiTransform->getOrigin();
		XMVECTORF32 color = { m_color.x, m_color.y, m_color.z, m_color.w };

		m_font->DrawString(&spriteBatch, std::wstring(m_text.begin(), m_text.end()).c_str(), position, color, sinf(XMConvertToRadians(rotation)), origin);
	}
}

SpriteFont* GUITextComponent::getFont() const
{
	return m_font;
}

void GUITextComponent::setFont(SpriteFont* font)
{
	m_font = font;
}

std::string GUITextComponent::getText() const
{
	return m_text;
}

void GUITextComponent::setText(std::string text)
{
	m_text = text;
}

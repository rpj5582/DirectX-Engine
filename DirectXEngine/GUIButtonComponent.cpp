#include "GUIButtonComponent.h"

#include "GUITransform.h"

using namespace DirectX;

GUIButtonComponent::GUIButtonComponent(Entity& entity) : GUISpriteComponent(entity)
{
	m_onClick = nullptr;
	m_isClicking = false;

	m_font = nullptr;
	m_fontID = "";
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

	setFont("default");
}

void GUIButtonComponent::loadFromJSON(rapidjson::Value& dataObject)
{
	GUISpriteComponent::loadFromJSON(dataObject);

	rapidjson::Value::MemberIterator font = dataObject.FindMember("font");
	if (font != dataObject.MemberEnd())
	{
		setFont(font->value.GetString());
	}

	rapidjson::Value::MemberIterator text = dataObject.FindMember("text");
	if (text != dataObject.MemberEnd())
	{
		setText(text->value.GetString());
	}

	rapidjson::Value::MemberIterator textColor = dataObject.FindMember("textColor");
	if (textColor != dataObject.MemberEnd())
	{
		setTextColor(XMFLOAT4(textColor->value["r"].GetFloat(), textColor->value["g"].GetFloat(), textColor->value["b"].GetFloat(), textColor->value["a"].GetFloat()));
	}
}

void GUIButtonComponent::saveToJSON(rapidjson::Writer<rapidjson::StringBuffer>& writer)
{
	Component::saveToJSON(writer);

	writer.Key("font");
	writer.String(m_fontID.c_str());

	writer.Key("text");
	writer.String(m_text.c_str());

	writer.Key("textColor");
	writer.StartObject();

	writer.Key("r");
	writer.Double(m_textColor.x);

	writer.Key("g");
	writer.Double(m_textColor.y);

	writer.Key("b");
	writer.Double(m_textColor.z);

	writer.Key("a");
	writer.Double(m_textColor.w);

	writer.EndObject();
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

DirectX::SpriteFont* GUIButtonComponent::getFont() const
{
	return m_font;
}

void GUIButtonComponent::setFont(std::string fontID)
{
	m_font = AssetManager::getFont(fontID);
	if (m_font)
	{
		m_fontID = fontID;
	}
	else
	{
		m_fontID = "";
		m_font = nullptr;
	}
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

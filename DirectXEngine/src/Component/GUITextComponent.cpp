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

	if(!m_font)
		setFont(AssetManager::getAsset<Font>(DEFAULT_FONT));
}

void GUITextComponent::initDebugVariables()
{
	GUIComponent::initDebugVariables();

	debugAddFont("Font", &m_font, nullptr, &debugGUITextComponentSetFont);
	debugAddString("Text", &m_text);
}

void GUITextComponent::loadFromJSON(rapidjson::Value& dataObject)
{
	GUIComponent::loadFromJSON(dataObject);

	rapidjson::Value::MemberIterator font = dataObject.FindMember("font");
	if (font != dataObject.MemberEnd())
	{
		setFont(AssetManager::getAsset<Font>(font->value.GetString()));
	}

	rapidjson::Value::MemberIterator text = dataObject.FindMember("text");
	if (text != dataObject.MemberEnd())
	{
		m_text = text->value.GetString();
	}
}

void GUITextComponent::saveToJSON(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer)
{
	GUIComponent::saveToJSON(writer);

	writer.Key("font");
	writer.String(m_font->getAssetID().c_str());

	writer.Key("text");
	writer.String(m_text.c_str());
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

		m_font->getSpriteFont()->DrawString(&spriteBatch, std::wstring(m_text.begin(), m_text.end()).c_str(), position, color, sinf(XMConvertToRadians(rotation)), origin);
	}
}

Font* GUITextComponent::getFont() const
{
	return m_font;
}

void GUITextComponent::setFont(Font* font)
{
	if (font)
		m_font = font;
	else
		m_font = AssetManager::getAsset<Font>(DEFAULT_FONT);
}

std::string GUITextComponent::getText() const
{
	return m_text;
}

void GUITextComponent::setText(std::string text)
{
	m_text = text;
}

void debugGUITextComponentSetFont(Component* component, const void* value)
{
	Font* font = *static_cast<Font*const*>(value);
	static_cast<GUITextComponent*>(component)->setFont(font);
}

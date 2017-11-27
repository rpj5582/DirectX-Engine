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

	setFont(DEFAULT_FONT);
}

void GUITextComponent::initDebugVariables()
{
	GUIComponent::initDebugVariables();

	Debug::entityDebugWindow->addVariableWithCallbacks(TW_TYPE_STDSTRING, "Font", this, &getGUITextComponentFontDebugEditor, &setGUITextComponentFontDebugEditor, this);
	Debug::entityDebugWindow->addVariable(&m_text, TW_TYPE_STDSTRING, "Text", this);
}

void GUITextComponent::loadFromJSON(rapidjson::Value& dataObject)
{
	GUIComponent::loadFromJSON(dataObject);

	rapidjson::Value::MemberIterator font = dataObject.FindMember("font");
	if (font != dataObject.MemberEnd())
	{
		setFont(font->value.GetString());
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
	writer.String(m_fontID.c_str());

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

std::string GUITextComponent::getFontID() const
{
	return m_fontID;
}

void GUITextComponent::setFont(std::string fontID)
{
	m_font = AssetManager::getAsset<Font>(fontID);
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

std::string GUITextComponent::getText() const
{
	return m_text;
}

void GUITextComponent::setText(std::string text)
{
	m_text = text;
}

void TW_CALL getGUITextComponentFontDebugEditor(void* value, void* clientData)
{
	GUITextComponent* component = static_cast<GUITextComponent*>(clientData);
	TwCopyStdStringToLibrary(*static_cast<std::string*>(value), component->getFontID());
}

void TW_CALL setGUITextComponentFontDebugEditor(const void* value, void* clientData)
{
	GUITextComponent* component = static_cast<GUITextComponent*>(clientData);
	component->setFont(*static_cast<const std::string*>(value));
}

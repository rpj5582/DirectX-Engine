#include "GUIButtonComponent.h"

#include "GUITransform.h"
#include "../Input.h"

using namespace DirectX;

GUIButtonComponent::GUIButtonComponent(Entity& entity) : GUISpriteComponent(entity)
{
	m_onClick = nullptr;

	m_font = nullptr;
	m_fontID = "";
	m_text = "";
	m_textColor = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
}

GUIButtonComponent::~GUIButtonComponent()
{
}

void GUIButtonComponent::init()
{
	GUISpriteComponent::init();

	setFont(DEFAULT_FONT);
}

void GUIButtonComponent::initDebugVariables()
{
	GUISpriteComponent::initDebugVariables();

	Debug::entityDebugWindow->addVariableWithCallbacks(TW_TYPE_STDSTRING, "Font", this, &getGUIButtonComponentFontDebugEditor, &setGUIButtonComponentFontDebugEditor, this);
	Debug::entityDebugWindow->addVariable(&m_text, TW_TYPE_STDSTRING, "Text", this);
	Debug::entityDebugWindow->addVariable(&m_textColor, TW_TYPE_COLOR4F, "Text Color", this);
}

void GUIButtonComponent::update(float deltaTime, float totalTime)
{
	if (Input::isMouseButtonPressed(MOUSE_LEFT))
	{
		if (!m_onClick) return;

		XMFLOAT2 mousePosition = Input::getMousePositon();

		GUITransform* guiTransform = entity.getComponent<GUITransform>();
		if (!guiTransform) return;

		if (guiTransform->containsPoint(mousePosition))
		{
			m_onClick();
		}
	}
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

void GUIButtonComponent::saveToJSON(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer)
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

	if (!m_texture) return;

	XMFLOAT2 position = guiTransform->getPosition();
	float rotation = guiTransform->getRotation();
	XMFLOAT2 size = guiTransform->getSize();
	XMFLOAT2 origin = guiTransform->getOrigin();

	XMVECTORF32 color = { m_color.x, m_color.y, m_color.z, m_color.w };
	XMVECTORF32 textColor = { m_textColor.x, m_textColor.y, m_textColor.z, m_textColor.w };
	spriteBatch.Draw(m_texture->getSRV(), position, nullptr, color, sinf(XMConvertToRadians(rotation)), origin, size);

	XMVECTOR originVector = XMLoadFloat2(&origin);
	XMVECTOR sizeVector = XMLoadFloat2(&size);
	XMStoreFloat2(&origin, XMVectorMultiply(originVector, sizeVector));

	m_font->getSpriteFont()->DrawString(&spriteBatch, std::wstring(m_text.begin(), m_text.end()).c_str(), position, textColor, sinf(XMConvertToRadians(rotation)), origin);
}

Font* GUIButtonComponent::getFont() const
{
	return m_font;
}

std::string GUIButtonComponent::getFontID() const
{
	return m_fontID;
}

void GUIButtonComponent::setFont(std::string fontID)
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

void TW_CALL getGUIButtonComponentFontDebugEditor(void* value, void* clientData)
{
	GUIButtonComponent* component = static_cast<GUIButtonComponent*>(clientData);
	TwCopyStdStringToLibrary(*static_cast<std::string*>(value), component->getFontID());
}

void TW_CALL setGUIButtonComponentFontDebugEditor(const void* value, void* clientData)
{
	GUIButtonComponent* component = static_cast<GUIButtonComponent*>(clientData);
	component->setFont(*static_cast<const std::string*>(value));
}

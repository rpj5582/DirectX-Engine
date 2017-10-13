#include "GUISpriteComponent.h"

#include "GUITransform.h"

using namespace DirectX;

GUISpriteComponent::GUISpriteComponent(Entity& entity) : GUIComponent(entity)
{
	m_textureSRV = nullptr;
	m_textureID = "";
}

GUISpriteComponent::~GUISpriteComponent()
{
}

void GUISpriteComponent::init()
{
	GUIComponent::init();

	setTexture("defaultGUI");
}

void GUISpriteComponent::loadFromJSON(rapidjson::Value& dataObject)
{
	GUIComponent::loadFromJSON(dataObject);

	rapidjson::Value::MemberIterator texture = dataObject.FindMember("texture");
	if (texture != dataObject.MemberEnd())
	{
		setTexture(texture->value.GetString());
	}
}

void GUISpriteComponent::saveToJSON(rapidjson::Writer<rapidjson::StringBuffer>& writer)
{
	Component::saveToJSON(writer);

	writer.Key("texture");
	writer.String(m_textureID.c_str());
}

void GUISpriteComponent::draw(DirectX::SpriteBatch& spriteBatch) const
{
	GUITransform* guiTransform = entity.getComponent<GUITransform>();
	if (!guiTransform) return;

	if (!m_textureSRV) return;

	XMFLOAT2 position = guiTransform->getPosition();
	float rotation = guiTransform->getRotation();
	XMFLOAT2 size = guiTransform->getSize();
	XMFLOAT2 origin = guiTransform->getOrigin();
	XMVECTORF32 color = { m_color.x, m_color.y, m_color.z, m_color.w };
	spriteBatch.Draw(m_textureSRV, position, nullptr, color, sinf(XMConvertToRadians(rotation)), origin, size);
}

ID3D11ShaderResourceView* GUISpriteComponent::getTextureSRV() const
{
	return m_textureSRV;
}

void GUISpriteComponent::setTexture(std::string textureID)
{
	m_textureSRV = AssetManager::getTexture(textureID);
	if (m_textureSRV)
	{
		m_textureID = textureID;
	}
	else
	{
		m_textureID = "";
		m_textureSRV = nullptr;
	}
}

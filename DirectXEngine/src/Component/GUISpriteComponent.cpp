#include "GUISpriteComponent.h"

#include "GUITransform.h"

using namespace DirectX;

GUISpriteComponent::GUISpriteComponent(Entity& entity) : GUIComponent(entity)
{
	m_texture = nullptr;
	m_textureID = "";
}

GUISpriteComponent::~GUISpriteComponent()
{
}

void GUISpriteComponent::init()
{
	GUIComponent::init();

	setTexture(DEFAULT_TEXTURE_WHITE);
}

void GUISpriteComponent::initDebugVariables()
{
	GUIComponent::initDebugVariables();

	Debug::entityDebugWindow->addVariableWithCallbacks(TW_TYPE_STDSTRING, "Texture", this, &getGUISpriteComponentTextureDebugEditor, &setGUISpriteComponentTextureDebugEditor, this);
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

#if defined(DEBUG) || defined(_DEBUG)
	DebugEntity* debugIcon = entity.getDebugIcon();
	if (debugIcon)
	{
		guiTransform = debugIcon->getGUITransform();
	}
#endif

	if (!guiTransform) return;
	if (!m_texture) return;

	XMFLOAT2 position = guiTransform->getPosition();
	float rotation = guiTransform->getRotation();
	XMFLOAT2 origin = guiTransform->getOrigin();
	XMVECTORF32 color = { m_color.x, m_color.y, m_color.z, m_color.w };

	XMFLOAT2 size = guiTransform->getSize();
	XMFLOAT2 textureSize = XMFLOAT2((float)m_texture->getWidth(), (float)m_texture->getHeight());

	XMVECTOR textureSizeVec = XMLoadFloat2(&textureSize);

	XMVECTOR scaledSizeVec = XMVectorDivide(XMLoadFloat2(&size), textureSizeVec);
	XMFLOAT2 scaledSize;
	XMStoreFloat2(&scaledSize, scaledSizeVec);

	XMVECTOR scaledOriginVec = XMVectorMultiply(XMLoadFloat2(&origin), textureSizeVec);
	XMFLOAT2 scaledOrigin;
	XMStoreFloat2(&scaledOrigin, scaledOriginVec);

	spriteBatch.Draw(m_texture->getSRV(), position, nullptr, color, sinf(XMConvertToRadians(rotation)), scaledOrigin, scaledSize);
}

Texture* GUISpriteComponent::getTexture() const
{
	return m_texture;
}

std::string GUISpriteComponent::getTextureID() const
{
	return m_textureID;
}

void GUISpriteComponent::setTexture(std::string textureID)
{
	if (textureID == "")
	{
		m_texture = AssetManager::getAsset<Texture>(DEFAULT_TEXTURE_WHITE);
		m_textureID = DEFAULT_TEXTURE_WHITE;
		return;
	}

	Texture* texture = AssetManager::getAsset<Texture>(textureID);
	if (texture)
	{
		m_texture = texture;
		m_textureID = textureID;
	}
}

void TW_CALL getGUISpriteComponentTextureDebugEditor(void* value, void* clientData)
{
	GUISpriteComponent* component = static_cast<GUISpriteComponent*>(clientData);
	std::string* textureInputField = static_cast<std::string*>(value);
	TwCopyStdStringToLibrary(*textureInputField, component->getTextureID());
}

void TW_CALL setGUISpriteComponentTextureDebugEditor(const void* value, void* clientData)
{
	GUISpriteComponent* component = static_cast<GUISpriteComponent*>(clientData);
	component->setTexture(*static_cast<const std::string*>(value));
}

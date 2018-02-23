#include "GUISpriteComponent.h"

#include "GUITransform.h"

using namespace DirectX;

GUISpriteComponent::GUISpriteComponent(Entity& entity) : GUIComponent(entity)
{
	m_texture = nullptr;
}

GUISpriteComponent::~GUISpriteComponent()
{
}

void GUISpriteComponent::init()
{
	GUIComponent::init();

	setTexture(AssetManager::getAsset<Texture>(DEFAULT_TEXTURE_WHITE));
}

void GUISpriteComponent::initDebugVariables()
{
	GUIComponent::initDebugVariables();

	debugAddTexture("Texture", &m_texture, nullptr, &debugGUISpriteComponentSetTexture);
}

void GUISpriteComponent::loadFromJSON(rapidjson::Value& dataObject)
{
	GUIComponent::loadFromJSON(dataObject);

	rapidjson::Value::MemberIterator texture = dataObject.FindMember("texture");
	if (texture != dataObject.MemberEnd())
	{
		setTexture(AssetManager::getAsset<Texture>(texture->value.GetString()));
	}
}

void GUISpriteComponent::saveToJSON(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer)
{
	Component::saveToJSON(writer);

	writer.Key("texture");
	writer.String(m_texture->getAssetID().c_str());
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

void GUISpriteComponent::setTexture(Texture* texture)
{
	if (texture)
		m_texture = texture;
	else
		m_texture = AssetManager::getAsset<Texture>(DEFAULT_TEXTURE_WHITE);
}

void debugGUISpriteComponentSetTexture(Component* component, const void* value)
{
	Texture* texture = *static_cast<Texture*const*>(value);
	static_cast<GUISpriteComponent*>(component)->setTexture(texture);
}

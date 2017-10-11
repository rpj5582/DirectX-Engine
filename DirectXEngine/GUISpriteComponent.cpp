#include "GUISpriteComponent.h"

#include "Scene.h"

using namespace DirectX;

GUISpriteComponent::GUISpriteComponent(Scene* scene, Entity entity) : GUIComponent(scene, entity)
{
	m_textureSRV = nullptr;
}

GUISpriteComponent::~GUISpriteComponent()
{
}

void GUISpriteComponent::init()
{
	GUIComponent::init();

	m_textureSRV = AssetManager::getTexture("defaultGUI");
}

void GUISpriteComponent::draw(Scene* scene, DirectX::SpriteBatch* spriteBatch) const
{
	GUITransform* guiTransform = scene->getComponentOfEntity<GUITransform>(entity);
	if (!guiTransform) return;

	if (!m_textureSRV) return;

	XMFLOAT2 position = guiTransform->getPosition();
	float rotation = guiTransform->getRotation();
	XMFLOAT2 size = guiTransform->getSize();
	XMFLOAT2 origin = guiTransform->getOrigin();
	XMVECTORF32 color = { m_color.x, m_color.y, m_color.z, m_color.w };
	spriteBatch->Draw(m_textureSRV, position, nullptr, color, sinf(XMConvertToRadians(rotation)), origin, size);
}

ID3D11ShaderResourceView* GUISpriteComponent::getTextureSRV() const
{
	return m_textureSRV;
}

void GUISpriteComponent::setTextureSRV(ID3D11ShaderResourceView* textureSRV)
{
	m_textureSRV = textureSRV;
}

#include "GUISpriteComponent.h"

GUISpriteComponent::GUISpriteComponent(Scene* scene, unsigned int entity) : GUIComponent(scene, entity)
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

void GUISpriteComponent::update(float deltaTime, float totalTime)
{
}

ID3D11ShaderResourceView* GUISpriteComponent::getTextureSRV() const
{
	return m_textureSRV;
}

void GUISpriteComponent::setTextureSRV(ID3D11ShaderResourceView* textureSRV)
{
	m_textureSRV = textureSRV;
}

#include "GUIComponent.h"

GUIComponent::GUIComponent(Scene* scene, unsigned int entity) : Component(scene, entity)
{
	m_textureSRV = nullptr;
}

GUIComponent::~GUIComponent()
{
}

void GUIComponent::init()
{
	m_textureSRV = AssetManager::getTexture("defaultGUI");
}

void GUIComponent::update(float deltaTime, float totalTime)
{
}

ID3D11ShaderResourceView* GUIComponent::getTextureSRV() const
{
	return m_textureSRV;
}

void GUIComponent::setTextureSRV(ID3D11ShaderResourceView* textureSRV)
{
	m_textureSRV = textureSRV;
}

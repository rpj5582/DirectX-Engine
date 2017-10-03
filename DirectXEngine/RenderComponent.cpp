#include "RenderComponent.h"

RenderComponent::RenderComponent(Scene* scene, unsigned int entity) : Component(scene, entity)
{
	m_material = nullptr;
	m_renderStyle = RenderStyle::SOLID;
}

RenderComponent::~RenderComponent()
{
}

void RenderComponent::init()
{
	m_material = AssetManager::getMaterial("default");
}

void RenderComponent::update(float deltaTime, float totalTime)
{
}

Material* RenderComponent::getMaterial() const
{
	return m_material;
}

void RenderComponent::setMaterial(Material* material)
{
	m_material = material;
}

RenderStyle RenderComponent::getRenderStyle() const
{
	return m_renderStyle;
}

void RenderComponent::setRenderStyle(RenderStyle renderStyle)
{
	m_renderStyle = renderStyle;
}

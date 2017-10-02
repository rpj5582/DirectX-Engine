#include "RenderComponent.h"

RenderComponent::RenderComponent(Scene* scene, unsigned int entity) : Component(scene, entity)
{
	m_material = nullptr;
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

void RenderComponent::changeMaterial(Material* material)
{
	m_material = material;
}

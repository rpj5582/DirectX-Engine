#include "DebugEntity.h"

#include "Component/GUITransform.h"
#include "Component/GUIDebugSpriteComponent.h"

using namespace DirectX;

DebugEntity::DebugEntity(Entity& entity)
{
	enabled = true;

	m_guiTransform = new GUITransform(entity);
	m_guiTransform->init();

	m_guiDebugSpriteComponent = new GUIDebugSpriteComponent(entity);
	m_guiDebugSpriteComponent->init();

	m_guiTransform->setOrigin(XMFLOAT2(0.5f, 0.5f));
	m_guiTransform->setSize(XMFLOAT2((float)m_guiDebugSpriteComponent->getTexture()->getWidth(), (float)m_guiDebugSpriteComponent->getTexture()->getHeight()));
}

DebugEntity::~DebugEntity()
{
	delete m_guiTransform;
	delete m_guiDebugSpriteComponent;
}

GUITransform* DebugEntity::getGUITransform() const
{
	return m_guiTransform;
}

GUIDebugSpriteComponent* DebugEntity::getGUIDebugSpriteComponent() const
{
	return m_guiDebugSpriteComponent;
}

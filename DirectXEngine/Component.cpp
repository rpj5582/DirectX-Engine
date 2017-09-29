#include "Component.h"

Component::Component(Entity* entity)
{
	m_entity = entity;
}

Component::~Component()
{
}

Entity* Component::getEntity() const
{
	return m_entity;
}

#include "Component.h"

#include "Scene.h"

Component::Component(Scene* scene, unsigned int entity)
{
	this->scene = scene;
	this->entity = entity;

	enabled = true;
}

Component::~Component()
{
}

void Component::onMouseDown(WPARAM buttonState, int x, int y)
{
}

void Component::onMouseUp(WPARAM buttonState, int x, int y)
{
}

void Component::onMouseMove(WPARAM wParam, int x, int y)
{
}

void Component::onMouseWheel(float wheelDelta, int x, int y)
{
}

unsigned int Component::getEntity() const
{
	return entity;
}

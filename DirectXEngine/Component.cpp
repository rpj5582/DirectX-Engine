#include "Component.h"

Component::Component(Scene* scene, Entity entity)
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

void Component::init()
{
}

void Component::update(float deltaTime, float totalTime)
{
}

Entity Component::getEntity() const
{
	return entity;
}

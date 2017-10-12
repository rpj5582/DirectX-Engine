#include "Component.h"

Component::Component(Entity& entity) : entity(entity), enabled(true)
{
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

void Component::lateUpdate(float deltaTime, float totalTime)
{
}

void Component::loadFromJSON(rapidjson::Value& dataObject)
{
	rapidjson::Value::MemberIterator componentEnabled = dataObject.FindMember("enabled");
	if (componentEnabled != dataObject.MemberEnd())
	{
		enabled = componentEnabled->value.GetBool();
	}
}

Entity& Component::getEntity() const
{
	return entity;
}

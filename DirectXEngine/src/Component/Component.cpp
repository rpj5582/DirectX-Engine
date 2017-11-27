#include "Component.h"

#include "ComponentRegistry.h"

Component::Component(Entity& entity) : entity(entity), enabled(true)
{
	typeName = "";
}

Component::~Component()
{
}

void Component::init()
{
	typeName = ComponentRegistry::getTypeName(typeid(*this));
}

void Component::initDebugVariables()
{
	Debug::entityDebugWindow->addVariable(&enabled, TW_TYPE_BOOLCPP, "Enabled", this);
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

void Component::saveToJSON(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer)
{
	writer.Key("enabled");
	writer.Bool(enabled);
}

void Component::onSceneLoaded()
{
}

Entity& Component::getEntity() const
{
	return entity;
}

std::string Component::getName() const
{
	return typeName;
}

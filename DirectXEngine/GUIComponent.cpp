#include "GUIComponent.h"

using namespace DirectX;

GUIComponent::GUIComponent(Entity& entity) : Component(entity)
{
	m_color = XMFLOAT4();
}

GUIComponent::~GUIComponent()
{
}

void GUIComponent::init()
{
	Component::init();

	m_color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	Debug::entityDebugWindow->addVariable(&m_color, TW_TYPE_COLOR4F, "Color", typeName, entity.getName());
}

void GUIComponent::loadFromJSON(rapidjson::Value& dataObject)
{
	Component::loadFromJSON(dataObject);

	rapidjson::Value::MemberIterator color = dataObject.FindMember("color");
	if (color != dataObject.MemberEnd())
	{
		m_color = XMFLOAT4(color->value["r"].GetFloat(), color->value["g"].GetFloat(), color->value["b"].GetFloat(), color->value["a"].GetFloat());
	}
}

void GUIComponent::saveToJSON(rapidjson::Writer<rapidjson::StringBuffer>& writer)
{
	Component::saveToJSON(writer);

	writer.Key("color");
	writer.StartObject();

	writer.Key("r");
	writer.Double(m_color.x);

	writer.Key("g");
	writer.Double(m_color.y);

	writer.Key("b");
	writer.Double(m_color.z);

	writer.Key("a");
	writer.Double(m_color.w);

	writer.EndObject();
}

DirectX::XMFLOAT4 GUIComponent::getColor() const
{
	return m_color;
}

void GUIComponent::setColor(DirectX::XMFLOAT4 color)
{
	m_color = color;
}

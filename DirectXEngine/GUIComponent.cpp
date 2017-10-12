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
	m_color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
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

DirectX::XMFLOAT4 GUIComponent::getColor() const
{
	return m_color;
}

void GUIComponent::setColor(DirectX::XMFLOAT4 color)
{
	m_color = color;
}

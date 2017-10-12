#include "GUITransform.h"

using namespace DirectX;

GUITransform::GUITransform(Entity& entity) : Component(entity)
{
	m_position = XMFLOAT2();
	m_rotation = 0.0f;
	m_size = XMFLOAT2();
	m_origin = XMFLOAT2();
}

GUITransform::~GUITransform()
{
}

void GUITransform::init()
{
	m_size = XMFLOAT2(1.0f, 1.0f);
}

void GUITransform::loadFromJSON(rapidjson::Value& dataObject)
{
	Component::loadFromJSON(dataObject); 

	rapidjson::Value::MemberIterator position = dataObject.FindMember("position");
	rapidjson::Value::MemberIterator rotation = dataObject.FindMember("rotation");
	rapidjson::Value::MemberIterator size = dataObject.FindMember("size");
	rapidjson::Value::MemberIterator origin = dataObject.FindMember("origin");

	if (position != dataObject.MemberEnd())
	{
		m_position = XMFLOAT2(position->value["x"].GetFloat(), position->value["y"].GetFloat());
	}

	if (rotation != dataObject.MemberEnd())
	{
		m_rotation = rotation->value.GetFloat();
	}

	if (size != dataObject.MemberEnd())
	{
		m_size = XMFLOAT2(size->value["x"].GetFloat(), size->value["y"].GetFloat());
	}

	if (origin != dataObject.MemberEnd())
	{
		m_origin = XMFLOAT2(origin->value["x"].GetFloat(), origin->value["y"].GetFloat());
	}
}

bool GUITransform::containsPoint(int x, int y) const
{
	if (x >= m_position.x && x <= m_position.x + m_size.x &&
		y >= m_position.y && y <= m_position.y + m_size.y)
		return true;

	return false;
}

DirectX::XMFLOAT2 GUITransform::getPosition() const
{
	return m_position;
}

void GUITransform::setPosition(XMFLOAT2 position)
{
	m_position = position;
}

void GUITransform::move(XMFLOAT2 delta)
{
	XMVECTOR pos = XMLoadFloat2(&m_position);
	XMVECTOR del = XMLoadFloat2(&delta);

	pos = XMVectorAdd(pos, del);
	XMStoreFloat2(&m_position, pos);
}

void GUITransform::moveX(float deltaX)
{
	m_position.x += deltaX;
}

void GUITransform::moveY(float deltaY)
{
	m_position.y += deltaY;
}

float GUITransform::getRotation() const
{
	return m_rotation;
}

void GUITransform::setRotation(float degrees)
{
	m_rotation = degrees;
}

void GUITransform::rotate(float degrees)
{
	m_rotation += degrees;
}

XMFLOAT2 GUITransform::getSize() const
{
	return m_size;
}

void GUITransform::setSize(XMFLOAT2 size)
{
	m_size = size;
}

XMFLOAT2 GUITransform::getOrigin() const
{
	return m_origin;
}

void GUITransform::setOrigin(XMFLOAT2 origin)
{
	m_origin = origin;
}

#include "GUITransform.h"

using namespace DirectX;

GUITransform::GUITransform(Scene* scene, unsigned int entity) : Component(scene, entity)
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

void GUITransform::update(float deltaTime, float totalTime)
{
}

DirectX::XMFLOAT2 GUITransform::getPosition() const
{
	return m_position;
}

void GUITransform::setPosition(DirectX::XMFLOAT2 position)
{
	m_position = position;
}

void GUITransform::move(DirectX::XMFLOAT2 delta)
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

DirectX::XMFLOAT2 GUITransform::getSize() const
{
	return m_size;
}

void GUITransform::setSize(DirectX::XMFLOAT2 size)
{
	m_size = size;
}

DirectX::XMFLOAT2 GUITransform::getOrigin() const
{
	return m_origin;
}

void GUITransform::setOrigin(XMFLOAT2 origin)
{
	m_origin = origin;
}
